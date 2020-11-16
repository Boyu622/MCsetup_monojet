import os,sys

MXd_array = []
MY1_array = []

infile = open("paths_and_parameters.dat","r")
for line in infile:
    print line
    if "MXd_array" in line:
        for ele in line.split("=")[1].split("[")[1].split("]")[0].split(","):
            MXd_array.append(int(ele))
    if "MY1_array" in line:
        for ele in line.split("=")[1].split("[")[1].split("]")[0].split(","):
            MY1_array.append(int(ele))
    if "model" in line: model = line.split("=")[1].split("\n")[0].strip()
    if "ebeam" in line: ebeam = line.split("=")[1].split("\n")[0].strip()
    if "nevents" in line: nevents = line.split("=")[1].split("\n")[0].strip()
    if "ptj" in line: ptj = line.split("=")[1].split("\n")[0].strip()
    if "Qcut" in line: Qcut = line.split("=")[1].split("\n")[0].strip()
infile.close()
orgdir = os.getcwd()
jobdir = orgdir + "/results/condor_job"

for MXd in MXd_array:
    for MY1 in MY1_array:
        tag = "med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s"%(str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut)
        logdir = orgdir + "/results/%s"%tag
        os.system("mkdir -p %s"%logdir)
        filename=jobdir+"/run_%s.sh"%tag
        f=open(filename,'w')
        f.write("#!/bin/bash \n")
        f.write("cd "+orgdir+"\n")
        f.write("export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase \n")
        f.write("source ${ATLAS_LOCAL_ROOT_BASE}/user/atlasLocalSetup.sh \n")
        f.write("python mg5nlo_launch_single.py %s %s %s %s %s %s %s"%(str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut))
        f.close()

        output =logdir+"/" + tag + ".out"
        filename_sub=jobdir+"/sub_" + tag + ".sub"
        f_sub=open(filename_sub,'w')
        f_sub.write("universe = vanilla \n")
        f_sub.write("executable  = " + filename + " \n")
        f_sub.write("output  = " + output + " \n")
        f_sub.write('requirements = (OpSysAndVer =?= "CentOS7")\n')
        f_sub.write("+JobFlavour = \"nextweek\" \n")
        f_sub.write("queue \n")
        f_sub.close()
        cmd="condor_submit SUB_FILE "+filename_sub+" \n"
        print cmd
        os.system(cmd)


