###################################################################################################################
####                                     Hi1711 chip solution固件SOC签名工具                                   ####
####                            主要组成有配置文件、HASH准备工具、签名工具，由SHELL脚本实现                    ####
###################################################################################################################

一、关键文件说明：
	xx_rsa_4096.cfg：        RSA4096签名配置文件（xx:l0fw,l1fw,u-boot）
	xx_rsa_2048.cfg：        RSA2048签名配置文件（xx:l0fw,l1fw,u-boot），现已弃用
	prepare_code_sign_data： HASH准备工具，用于计算出固件的hash
	generate_sign_image：    签名工具，用于生成最终的固件


一、签名步骤：
	1. 运行 ./prepare_code_sign_data xx.bin 1(version) (l0fw,l1fw,u-boot) 生成 xx_hash_sign.bin
	2. 将xx_hash_sign.bin发给签名中心签名生成xx_hash_sign.bin.rsa 并且将 rsacert.cer放到当前目录
	3. 运行 ./generate_sign_image xx_rsa_4096.cfg 生成最终的xx_rsa_4096.bin
	
二、xxx.cfg配置文件修改说明
	配置文件中一般修改用于单签、双签或者不做签名的最终文件生成，其他字段客户无需修改，如有需要，后续版本说明。
	修改为签名模式的字段主要由FW_SECTION_LIST和FW_SECTION_FILES，修改示例如下：
	1. 不做签名：
		export FW_SECTION_LIST="padding padding padding padding padding padding"
		export FW_SECTION_FILES="padding padding padding padding padding padding"
	2. 单签：
		export FW_SECTION_LIST="host_root_pubk host_subkey_cert host_code_sign padding padding padding"
		export FW_SECTION_FILES="rsa_host_rtpubk_4096_C.bin rsacert.cer l0fw_hash_sign.bin.rsa padding padding padding"
	3. 双签：
		export FW_SECTION_LIST="host_root_pubk host_subkey_cert host_code_sign guest_root_pubk guest_subkey_cert guest_code_sign"
		export FW_SECTION_FILES="rsa_host_rtpubk_4096_C.bin rsacert.cer l1fw_hash_sign.bin.rsa rsa_guest_rtpubk_4096_C.bin rsacert.cer l1fw_hash_sign.bin.rsa"
		

三、CMS厂商根证书HASH填充说明
	CMS厂商根证书HASH填充由prepare_code_sign_data完成，会在计算固件HASH之前填充好，根证书文件名及填充位置均在该脚本文件中修改：
	1. 填充位置一般随l1fw.bin版本一起修改，版本未做说明，不能修改；
	2. 根证书文件名客户可以根据自己的根证书文件名称自行修改。