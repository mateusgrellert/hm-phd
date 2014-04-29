import os
import sys

def get_cfg_specs(args):
	cfg = open(args[1], "r")
	lines = cfg.readlines()
	cfg.close()

	specs = [0,0,0]

	for l in lines:
		if "InputFile" in l:
			t = l.split(":")[1] 		# ../../origCfP/BasketballDrill_832x480_50.yuv
			t = t.split("/")[-1] 		# BasketballDrill_832x480_50.yuv
			t = t.split("_")[0]		# BasketballDrill
			specs[0] =  t

	t = args[2].split("/")[-1]		# encoder_randomaccess_main.cfg
	t = t.split(".")[0]			# encoder_randomaccess_main
	specs[1] = t
	specs[2] = args[3]
	return specs


cmd_line = "./TAppEncoderStatic"

if len(sys.argv) < 4:
	print "\n\tUsage: python rungprof.py [sequence cfg file] [RA/LD cfg file] [QP] [frame count](optional)\n"
	exit()
else:
	yuv_cfg = sys.argv[1]
	enc_cfg = sys.argv[2]
	qp = sys.argv[3]

cmd_line += " -c " + yuv_cfg + " -c " + enc_cfg + " --QP=" + qp

if len(sys.argv) == 5:
	num_frames = sys.argv[4]
	cmd_line += " --FramesToBeEncoded=" + num_frames

print "\rRunning command:"+cmd_line

os.system(cmd_line) # run HM with configs

specs = get_cfg_specs(sys.argv)

gprof_name = "_".join(specs) # rename gprof output

os.system("gprof TAppEncoderStatic gmon.out > gprof.out")
os.system("mv gprof.out ../gprof/" + gprof_name + ".gprof")
	



