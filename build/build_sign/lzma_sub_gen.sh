#!/bin/bash
# Copyright © Hisilicon Technologies. Co, Ltd. 2023-2023. All rights reserved

set -e

file=$1
final_file=$2
stage_file=.staging.bin
sub_file_list=''
sub_src_offset_list=''
sub_src_size_list=''
sub_lzma_offset_list=''
sub_lzma_size_list=''
sub_file_cnt=0
magic=0xddcc0022
final_file_max_size=$((164*1024))

function split_file()
{
	echo "Start Split File $file!"
	
	sub_size_template=(65536 65536 65536 32768 16384 8192 4096 2048)
    
	left_size=$(stat --format=%s $file)
	skip_size=0
	index=0
	for sub_size in ${sub_size_template[*]}; do
		if [ $left_size -ge $sub_size ]; then
			index=$(($index+1))
			echo ">>>>left_size:$left_size, cut $sub_size"
			echo "dd if=$file of=$file.$index bs=1 count=$sub_size skip=$skip_size"
			dd if=$file of=$file.$index bs=1 count=$sub_size skip=$skip_size
			
			sub_file_list=$sub_file_list" "$file.$index
			sub_src_size_list=$sub_src_size_list" "$sub_size
			
			left_size=$(($left_size-$sub_size))
			skip_size=$(($skip_size+$sub_size))
		fi
	done
    
	if [ $left_size -ne 0 ]; then
		index=$(($index+1))
		echo ">>>>left_size:$left_size, cut $sub_size"
		echo "dd if=$file of=$file.$index bs=1 count=$left_size skip=$skip_size"
		dd if=$file of=$file.$index bs=1 skip=$skip_size
		sub_file_list=$sub_file_list" "$file.$index
		sub_src_size_list=$sub_src_size_list" 2048"
	fi
	
	sub_file_cnt=$index
	
	echo "Generate"$sub_file_list
}

function lzma_files()
{
	echo "Start Lzma Files $sub_file_list!"

	for sub_file in $sub_file_list; do
		echo $sub_file
		lzma $sub_file
		sub_lzma_size_list=$sub_lzma_size_list" "$(stat --format=%s $sub_file.lzma)
	done
}

#
# U32 VALUE to little endian hex string(little endian byte array)
# $1 - word value string
# 
function u32_to_hex_byte_array()
{
	# Convert u32 value to string format xx:xx:xx:xx
	# e.g: u32 value = 123(decimal)
	#      converted string = 00:00:00:7b
	val=$(printf "%08x" $1 | cut -c 1-2,3-4,5-6,7-8 --output-delimiter=':')
	# Convert to little endian byte array
	byte0=$(echo $val | cut -d':' -f4)
	byte1=$(echo $val | cut -d':' -f3)
	byte2=$(echo $val | cut -d':' -f2)
	byte3=$(echo $val | cut -d':' -f1)
	echo $byte0$byte1$byte2$byte3
}

function get_size_offset()
{
	echo "Start Get Size and Offset!"
		
	head_size=$((8+$sub_file_cnt*16))
	first_src_size=$(echo "$sub_src_size_list" | awk '{print $i}' i=1)
	
	sub_lzma_offset=0
	sub_src_offset=0
	
	last_sub_lzma_offset=0
	last_sub_lzma_size=0
	for index in $(seq 1 $sub_file_cnt); do
		sub_src_size=$(echo "$sub_src_size_list" | awk '{print $i}' i=$index)
		
		#压缩后的分段需要先增加源文件大小，即每一段压缩后的数据在内存上布局应为该段解压后的位置，布局示意图如下
		#1.src  |2.src  |3.src  |4.src  |...
		#       |1.lzma |2.lzma |3.lzma |4.lzma
		sub_lzma_offset=$(($sub_lzma_offset+$sub_src_size))
				
		if [ $index -eq 1 ]; then
			sub_lzma_offset_list=$sub_lzma_offset_list' '$head_size
		else
			temp_sub_lzma_offset=$(($sub_lzma_offset-$first_src_size))	
			last_sub_lzma_end=$(($last_sub_lzma_offset+$last_sub_lzma_size))
			if [ $last_sub_lzma_end -gt $temp_sub_lzma_offset ]; then
				temp_sub_lzma_offset=$last_sub_lzma_end
			fi
			sub_lzma_offset_list=$sub_lzma_offset_list' '$temp_sub_lzma_offset	
		fi
		sub_src_offset_list=$sub_src_offset_list' '$sub_src_offset
		
		sub_src_offset=$(($sub_src_offset+$sub_src_size))
		
		last_sub_lzma_offset=$(echo "$sub_lzma_offset_list" | awk '{print $i}' i=$index)
		last_sub_lzma_size=$(echo "$sub_lzma_size_list" | awk '{print $i}' i=$index)
	done
	
	echo "Lzma Sub File offset!"
	echo "$sub_lzma_offset_list"
	
	echo "Lzma Sub File Size!"
	echo "$sub_lzma_size_list"
	
	echo "Source Sub File offset!"
	echo "$sub_src_offset_list"
	
	echo "Source Sub File Size!"
	echo "$sub_src_size_list"
}

function generate_head()
{
	echo "Start Generate Head!"
	
	hex_byte_str=$(u32_to_hex_byte_array $magic)
	printf "$hex_byte_str%02x000000" $sub_file_cnt | xxd -r -p > $stage_file
		
	for index in $(seq 1 $sub_file_cnt); do
		sub_lzma_offset=$(echo "$sub_lzma_offset_list" | awk '{print $i}' i=$index)
		sub_lzma_size=$(echo "$sub_lzma_size_list" | awk '{print $i}' i=$index)
		sub_src_offset=$(echo "$sub_src_offset_list" | awk '{print $i}' i=$index)
		sub_src_size=$(echo "$sub_src_size_list" | awk '{print $i}' i=$index)
		
		echo $sub_src_size ">>" $sub_lzma_size
		
		#lzma section offset
		hex_byte_str=$(u32_to_hex_byte_array $sub_lzma_offset)
		printf $hex_byte_str | xxd -r -p >> $stage_file
		
		#lzma section size
		hex_byte_str=$(u32_to_hex_byte_array $sub_lzma_size)
		printf $hex_byte_str | xxd -r -p >> $stage_file
		
		#source section offset
		hex_byte_str=$(u32_to_hex_byte_array $sub_src_offset)
		printf $hex_byte_str | xxd -r -p >> $stage_file
		
		#source section size
		hex_byte_str=$(u32_to_hex_byte_array $sub_src_size)
		printf $hex_byte_str | xxd -r -p >> $stage_file
	done
}

function pack_files()
{
	echo "Start Packet File $file.sub.lzma!"
	
	echo "sub_file:"$sub_file
			
	for index in $(seq 1 $sub_file_cnt); do
		sub_file=$(echo "$sub_file_list" | awk '{print $i}' i=$index)
		sub_lzma_offset=$(echo "$sub_lzma_offset_list" | awk '{print $i}' i=$index)
		
		echo "padding sub_file:"$sub_file
		
		echo "dd if=$sub_file.lzma of=$stage_file bs=1 seek=$sub_lzma_offset"
		dd if=$sub_file.lzma of=$stage_file bs=1 seek=$sub_lzma_offset	
		
		rm -f $sub_file
		rm -f $sub_file.lzma 
	done
		
	final_file_size=$(stat --format=%s $stage_file)
	if [ $final_file_size -gt $final_file_max_size ]; then
		echo "Final file size is overide, $final_file_size, compress fail!"
		exit -1
	fi
	
	mv $stage_file $final_file
}

if [ ! -n "$1" ]; then
	echo "param 1 should not is input file, should not be empty!"
	exit -1
fi

if [ ! -n "$2" ]; then
	echo "param 2 should not is output file, should not be empty!"
	exit -1
fi

split_file
lzma_files
get_size_offset
generate_head
pack_files
echo "Generate file $file.sub.lzma Success"
