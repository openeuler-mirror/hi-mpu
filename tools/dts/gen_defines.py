#!/usr/bin/env python
# -*- coding: UTF-8 -*-
# Copyright (c) Huawei Technologies Co., Ltd. 2020-2023. All rights reserved.
#!/usr/bin/env python3
import argparse
from collections import defaultdict
import logging
import os
import pathlib
import pickle
import re
import sys
import operator as op
import string

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python-devicetree',
                                'src'))

from devicetree import edtlib

class LogFormatter(logging.Formatter):
    '''A log formatter that prints the level name in lower case,
    for compatibility with earlier versions of edtlib.'''

    def __init__(self):
        super().__init__(fmt='%(levelnamelower)s: %(message)s')

    def format(self, record):
        record.levelnamelower = record.levelname.lower()
        return super().format(record)

def read_isolate_property(folder_base):
    try:
        contexts = ""
        with open(folder_base) as file_txt:
            contexts = file_txt.readline()
        return contexts
    except FileNotFoundError:
        print(folder_base + " is not exsit")
    except IOError:
        print('except IOError')
    
    return ""

def main():
    global header_file
    global flash_area_num

    args = parse_args()

    setup_edtlib_logging()

    vendor_prefixes = {}
    for prefixes_file in args.vendor_prefixes:
        vendor_prefixes.update(edtlib.load_vendor_prefixes_txt(prefixes_file))

    try:
        edt = edtlib.EDT(args.dts, args.bindings_dirs,
                         # Suppress this warning if it's suppressed in dtc
                         warn_reg_unit_address_mismatch=
                             "-Wno-simple_bus_reg" not in args.dtc_flags,
                         default_prop_types=True,
                         infer_binding_for_paths=["/hi309x,user"],
                         werror=args.edtlib_Werror,
                         vendor_prefixes=vendor_prefixes)
    except edtlib.EDTError as e:
        sys.exit(f"devicetree error: {e}")

    flash_area_num = 0

    for compat, nodes in edt.compat2nodes.items():
        edt.compat2nodes[compat] = sorted(
            nodes, key=lambda node: 0 if node.status == "okay" else 1)

    # Create the generated header.
    with open(args.header_out, "w", encoding="utf-8") as header_file:

        # populate all z_path_id first so any children references will
        # work correctly.
        write_comment_h('Copyright (c) Huawei Technologies Co., Ltd. 2020-2020. All rights reserved.\n')
        write_comment_h('#ifndef __DEVICE_RESOURCE_H__',False)
        write_comment_h('#define __DEVICE_RESOURCE_H__',)

        contexts = read_isolate_property(args.isolate_text[0]).split(",")
        for node in sorted(edt.nodes, key=lambda node: node.dep_ordinal):
            node.z_path_id = node_z_path_id(node)

        # Check to see if we have duplicate "zephyr,memory-region" property values.
        regions = dict()
        for node in sorted(edt.nodes, key=lambda node: node.dep_ordinal):
            write_vanilla_props(node, contexts)
        
        write_comment_h('#endif /* __DEVICE_RESOURCE_H__ */')

    if args.edt_pickle_out:
        write_pickled_edt(edt, args.edt_pickle_out)


def setup_edtlib_logging():

    handler = logging.StreamHandler(sys.stderr)
    handler.setFormatter(LogFormatter())

    logger = logging.getLogger('edtlib')
    logger.setLevel(logging.WARNING)
    logger.addHandler(handler)

def node_z_path_id(node):
    # Return the node specific bit of the node's path identifier:
    #
    # - the root node's path "/" has path identifier "N"
    # - "/foo" has "N_S_foo"
    # - "/foo/bar" has "N_S_foo_S_bar"
    # - "/foo/bar@123" has "N_S_foo_S_bar_123"
    #
    # This is used throughout this file to generate macros related to
    # the node.

    components = ["N"]
    if node.parent is not None:
        components.extend(f"S_{str2ident_changes(component)}" for component in
                          node.path.split("/")[1:])
    
    return "_".join(components)

def parse_args():
    parser = argparse.ArgumentParser(allow_abbrev=False)
    parser.add_argument("--dts", required=True, help="DTS file")
    parser.add_argument("--dtc-flags",
                        help="'dtc' devicetree compiler flags, some of which "
                             "might be respected here")
    parser.add_argument("--bindings-dirs", nargs='+', required=True,
                        help="directory with bindings in YAML format, "
                        "we allow multiple")
    parser.add_argument("--header-out", required=True,
                        help="path to write header to")
    parser.add_argument("--edt-pickle-out",
                        help="path to write pickled edtlib.EDT object to")
    parser.add_argument("--vendor-prefixes", action='append', default=[],
                        help="vendor-prefixes.txt path; used for validation; "
                             "may be given multiple times")
    parser.add_argument("--edtlib-Werror", action="store_true",
                        help="if set, edtlib-specific warnings become errors. "
                             "(this does not apply to warnings shared "
                             "with dtc.)")
    parser.add_argument("--isolate-text", nargs='+', required=True,
                        help="directory with isloate cpus, "
                        "we allow multiple")

    return parser.parse_args()


def write_vanilla_props(node, contexts):
    # Writes macros for any and all properties defined in the
    # "properties" section of the binding for the node.
    #
    # This does generate macros for special properties as well, like
    # regs, etc. Just let that be rather than bothering to add
    # never-ending amounts of special case code here to skip special
    # properties. This function's macros can't conflict with
    # write_special_props() macros, because they're in different
    # namespaces. Special cases aren't special enough to break the rules.

    macro2val = {}
    for prop_name, prop in node.props.items():
        prop_id = str2ident(prop_name)
        macro=""
        for propertys in contexts:
            if op.eq(propertys, prop_id):
                str_result = node.z_path_id.rstrip(string.digits)
                macro = f"{str_result}_P_{prop_id}"
        val = prop2value(prop)
        if val is not None:
            # DT_N_<node-id>_P_<prop-id>
            macro2val[macro] = val

        if prop.enum_index is not None:
            # DT_N_<node-id>_P_<prop-id>_ENUM_IDX
            macro2val[macro.join("_ENUM_IDX")] = prop.enum_index
            spec = prop.spec

            if spec.enum_tokenizable:
                as_token = prop.val_as_token

                # DT_N_<node-id>_P_<prop-id>_ENUM_VAL_<val>_EXISTS 1
                macro2val[macro.join(f"_ENUM_VAL_{as_token}_EXISTS")] = 1
                # DT_N_<node-id>_P_<prop-id>_ENUM_TOKEN
                macro2val[macro.join("_ENUM_TOKEN")] = as_token

                if spec.enum_upper_tokenizable:
                    # DT_N_<node-id>_P_<prop-id>_ENUM_UPPER_TOKEN
                    macro2val[macro.join("_ENUM_UPPER_TOKEN")] = as_token.upper()
            else:
                # DT_N_<node-id>_P_<prop-id>_ENUM_VAL_<val>_EXISTS 1
                macro2val[macro.join(f"_ENUM_VAL_{prop.val}_EXISTS")] = 1

        if "phandle" in prop.type:
            macro2val.update(phandle_macros(prop, macro))

        elif "array" in prop.type:
            solve_type_array(node, macro2val, prop.val, macro, contexts)
        
    if macro2val:
        for macro, val in macro2val.items():
            for propertys in contexts:
                    if propertys in macro and type(val) == int:
                        out_dt_define(macro.upper(), val)
        

def solve_type_array(node, macro2val, prop_val, macro, contexts):
    channel_min = 0
    channel_max = 0
    for i, subval in enumerate(prop_val):
        str_result = str2ident_changes(node.z_path_id).split("_")

        if "channel" in macro:
            if i % 2 == 0:
                channel_min = subval
            else:
                channel_max = subval
                for j in range(channel_min, channel_max + 1):
                    res = macro.replace("channel", "domain")
                    macro2val[res + f"_CHANNEL{j}_EXISTS"] = 1
                continue
        
        for propertys in contexts:
            if propertys in macro and subval == 1 and "channel" not in macro:
                str_pattern = re.compile(r".*[0-9]$")
                if str_pattern.match(str_result[len(str_result) - 1]):

                    macro2val[macro + f"_{str_result[len(str_result) - 1]}_EXISTS"] = 1
                else:
                    macro2val[macro + f"_{str_result[len(str_result) - 1]}{i}_EXISTS"] = 1


def prop2value(prop):
    # Gets the macro value for property 'prop', if there is
    # a single well-defined C rvalue that it can be represented as.
    # Returns None if there isn't one.

    if prop.type == "string":
        return quote_str(prop.val)

    if prop.type == "int":
        return prop.val

    if prop.type == "boolean":
        return 1 if prop.val else 0

    if prop.type in ["array", "uint8-array"]:
        return list2init(f"{val} /* {hex(val)} */" for val in prop.val)

    if prop.type == "string-array":
        return list2init(quote_str(val) for val in prop.val)

    # phandle, phandles, phandle-array, path, compound: nothing
    return None


def phandle_macros(prop, macro):
    # Returns a dict of macros for phandle or phandles property 'prop'.
    #
    # The 'macro' argument is the N_<node-id>_P_<prop-id> bit.
    #
    # These are currently special because we can't serialize their
    # values without using label properties, which we're trying to get
    # away from needing in Zephyr. (Label properties are great for
    # humans, but have drawbacks for code size and boot time.)
    #
    # The names look a bit weird to make it easier for devicetree.h
    # to use the same macros for phandle, phandles, and phandle-array.

    ret = {}

    if prop.type == "phandle":
        # A phandle is treated as a phandles with fixed length 1.
        ret[f"{macro}"] = f"DT_{prop.val.z_path_id}"
        ret[f"{macro}_IDX_0"] = f"DT_{prop.val.z_path_id}"
        ret[f"{macro}_IDX_0_PH"] = f"DT_{prop.val.z_path_id}"
        ret[f"{macro}_IDX_0_EXISTS"] = 1
    elif prop.type == "phandles":
        for i, node in enumerate(prop.val):
            ret[f"{macro}_IDX_{i}"] = f"DT_{node.z_path_id}"
            ret[f"{macro}_IDX_{i}_PH"] = f"DT_{node.z_path_id}"
            ret[f"{macro}_IDX_{i}_EXISTS"] = 1
    elif prop.type == "phandle-array":
        for i, entry in enumerate(prop.val):
            if entry is None:
                # Unspecified element. The phandle-array at this index
                # does not point at a ControllerAndData value, but
                # subsequent indices in the array may.
                ret[f"{macro}_IDX_{i}_EXISTS"] = 0
                continue

            ret.update(controller_and_data_macros(entry, i, macro))

    return ret


def controller_and_data_macros(entry, i, macro):
    # Helper procedure used by phandle_macros().
    #
    # Its purpose is to write the "controller" (i.e. label property of
    # the phandle's node) and associated data macros for a
    # ControllerAndData.

    ret = {}
    data = entry.data

    # DT_N_<node-id>_P_<prop-id>_IDX_<i>_EXISTS
    ret[f"{macro}_IDX_{i}_EXISTS"] = 1
    # DT_N_<node-id>_P_<prop-id>_IDX_<i>_PH
    ret[f"{macro}_IDX_{i}_PH"] = f"DT_{entry.controller.z_path_id}"
    # DT_N_<node-id>_P_<prop-id>_IDX_<i>_VAL_<VAL>
    for cell, val in data.items():
        ret[f"{macro}_IDX_{i}_VAL_{str2ident(cell)}"] = val
        ret[f"{macro}_IDX_{i}_VAL_{str2ident(cell)}_EXISTS"] = 1

    if not entry.name:
        return ret

    name = str2ident(entry.name)
    # DT_N_<node-id>_P_<prop-id>_IDX_<i>_EXISTS
    ret[f"{macro}_IDX_{i}_EXISTS"] = 1
    # DT_N_<node-id>_P_<prop-id>_IDX_<i>_NAME
    ret[f"{macro}_IDX_{i}_NAME"] = quote_str(entry.name)
    # DT_N_<node-id>_P_<prop-id>_NAME_<NAME>_PH
    ret[f"{macro}_NAME_{name}_PH"] = f"DT_{entry.controller.z_path_id}"
    # DT_N_<node-id>_P_<prop-id>_NAME_<NAME>_EXISTS
    ret[f"{macro}_NAME_{name}_EXISTS"] = 1
    # DT_N_<node-id>_P_<prop-id>_NAME_<NAME>_VAL_<VAL>
    for cell, val in data.items():
        cell_ident = str2ident(cell)
        ret[f"{macro}_NAME_{name}_VAL_{cell_ident}"] = \
            f"DT_{macro}_IDX_{i}_VAL_{cell_ident}"
        ret[f"{macro}_NAME_{name}_VAL_{cell_ident}_EXISTS"] = 1

    return ret


def str2ident(s):
    # Converts 's' to a form suitable for (part of) an identifier

    return re.sub('[-,.@/+]', '_', s.lower())


def str2ident_changes(s):
    # Converts 's' to a form suitable for (part of) an identifier

    return re.sub("@.*?$", "", s.lower(), flags=re.S)


def list2init(l):
    # Converts 'l', a Python list (or iterable), to a C array initializer

    return "{" + ", ".join(l) + "}"


def out_dt_define(macro, val, width=None, deprecation_msg=None):
    ret = "DT_" + macro
    out_define(ret, val, width=width, deprecation_msg=deprecation_msg)
    return ret


def out_define(macro, val, width=None, deprecation_msg=None):
    # Helper for out_dt_define(). Outputs "#define <macro> <val>",
    # adds a deprecation message if given, and allocates whitespace
    # unless told not to.

    warn = fr' __WARN("{deprecation_msg}")' if deprecation_msg else ""

    if width:
        s = f"#define {macro.ljust(width)}{warn} {val}"
    else:
        s = f"#define {macro}{warn} {val}"

    print(s, file=header_file)


def write_comment_h(s, blank_before=True, flags=True):

    if "\n" in s:
        res = ["/*"]
        for line in s.splitlines():
            res.append(" *" if not line.strip() else " * " + line)
        res.append(" */")
        print("\n".join(res), file=header_file)
    else:
        print(s, file=header_file)

    if blank_before:
            print(file=header_file)

def escape(s):
    # Backslash-escapes any double quotes and backslashes in 's'

    # \ must be escaped before " to avoid double escaping
    return s.replace("\\", "\\\\").replace('"', '\\"')


def quote_str(s):
    # Puts quotes around 's' and escapes any double quotes and
    # backslashes within it

    return f'"{escape(s)}"'


def write_pickled_edt(edt, out_file):
    # Writes the edt object in pickle format to out_file.

    with open(out_file, 'wb') as f:
        pickle.dump(edt, f, protocol=4)

if __name__ == "__main__":
    main()