import os,sys

MXd_array = []
MY1_array = []

infile = open("launch_many_parameters.dat","r")
for line in infile:
    print line
    if "MXd_array" in line:
        for ele in line.split("=")[1].split("[")[1].split("]")[0].split(","):
            MXd_array.append(int(ele))
    if "MY1_array" in line:
        for ele in line.split("=")[1].split("[")[1].split("]")[0].split(","):
            MY1_array.append(int(ele))
    if "med_type" in line: med_type = line.split("=")[1].split("\n")[0].strip(" ")
    if "gDM" in line: gDM = line.split("=")[1].split("\n")[0].strip(" ")
    if "gq" in line: gq = line.split("=")[1].split("\n")[0].strip(" ")
    if "gl" in line: gl = line.split("=")[1].split("\n")[0].strip(" ")
    if "model_tag" in line: model = line.split("=")[1].split("\n")[0].strip(" ")
    if "ebeam" in line: ebeam = line.split("=")[1].split("\n")[0].strip(" ")
    if "nevents" in line: nevents = line.split("=")[1].split("\n")[0].strip(" ")
    if "xqcut" in line: xqcut = line.split("=")[1].split("\n")[0].strip(" ").strip("#only for lo")
    if "ptj" in line: ptj = line.split("=")[1].split("\n")[0].strip(" ").strip("#only for nlo")
    if "Qcut" in line: Qcut = line.split("=")[1].split("\n")[0].strip(" ").strip("#only for nlo")
    if "repeat" in line: repeat_index = line.split("=")[1].split("\n")[0].strip(" ")
    if "mode" in line: mode = line.split("=")[1].split("\n")[0].strip(" ")
infile.close()
orgdir = os.getcwd()
jobdir = orgdir + "/results/condor_job"

for i in range(int(repeat_index)):
  for MXd in MXd_array:
    for MY1 in MY1_array:
        if mode == "nlo" or mode == "NLO":
            tag0 = "nlo_med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s"%(str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut)
            tag = "nlo_med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s_repeat%s"%(str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut,i)
        if mode == "lo" or mode == "LO":
            tag0 = "lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s"%(str(MY1),str(MXd),model,ebeam,nevents,xqcut)
            tag = "lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s_repeat%s"%(str(MY1),str(MXd),model,ebeam,nevents,xqcut,i)
        logdir = orgdir + "/results/%s"%tag0
        os.system("mkdir -p %s"%logdir)
        filename=jobdir+"/run_%s.sh"%tag
        f=open(filename,'w')
        f.write("#!/bin/bash \n")
        f.write("cd "+orgdir+"\n")
        f.write("export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase \n")
        f.write("source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh \n")
        if mode == "nlo" or mode == "NLO":
            f.write("python mg5nlo_launch_single.py %s %s %s %s %s %s %s %s"%(str(MXd),str(MY1),model,ebeam,nevents,ptj,Qcut,i))
        if mode == "lo" or mode == "LO":
            f.write("python mg5lo_launch_single.py %s %s %s %s %s %s %s %s %s %s %s"%(str(MXd),str(MY1),med_type,gDM,gq,gl,model,ebeam,nevents,xqcut,i))
        f.close()

        output =logdir+"/" + tag + ".out"
        error  =logdir+"/" + tag + ".err"
        logname=logdir+"/" + tag + ".log"
        filename_sub=jobdir+"/sub_" + tag + ".sub"
        f_sub=open(filename_sub,'w')
        f_sub.write("universe = vanilla \n")
        f_sub.write("executable  = " + filename + " \n")
        f_sub.write("output  = " + output + " \n")
        f_sub.write("error  = " + error + " \n")
        f_sub.write("log  = " + logname + " \n")
        f_sub.write('requirements = (OpSysAndVer =?= "CentOS7")\n')
        if mode == "lo" or mode == "LO":
            f_sub.write("+JobFlavour = \"workday\" \n")
        if mode == "nlo" or mode == "NLO":
            f_sub.write("+JobFlavour = \"nextweek\" \n")
        f_sub.write("queue \n")
        f_sub.close()
        cmd="condor_submit SUB_FILE "+filename_sub+" \n"
        print cmd
        os.system(cmd)


