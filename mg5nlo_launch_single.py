import os,sys,pwd
from exclCalc import *

if len(sys.argv)<9:
    print "Please pass arguments following: python mg5nlo_launch_single.py {mDM} {Mmed} {model} {Ebeam_single} {nevents} {ptj} {Qcut} {repeat}"
    sys.exit()
else:
    MXd = sys.argv[1]
    MY1 = sys.argv[2]
    model = sys.argv[3]
    ebeam = sys.argv[4]
    nevents = sys.argv[5]
    ptj = sys.argv[6]
    Qcut = sys.argv[7]
    repeat_index = sys.argv[8]
    
if model == "A1" or model == "a1":
    gAXd, gVXd, gAq, gVq, gAl, gVl = 1.0, 0.0, 0.25, 0.0, 0.0, 0.0
    WY1 = totalWidthAxial(float(MY1),float(MXd),0.25,1.0,0.0)
if model == "A2" or model == "a2":
    gAXd, gVXd, gAq, gVq, gAl, gVl = 1.0, 0.0, 0.1, 0.0, 0.1, 0.0
    WY1 = totalWidthAxial(float(MY1),float(MXd),0.1,1.0,0.1)
if model == "V1" or model == "v1":
    gAXd, gVXd, gAq, gVq, gAl, gVl = 0.0, 1.0, 0.0, 0.25, 0.0, 0.0
    WY1 = totalWidthVector(float(MY1),float(MXd),0.25,1.0,0.0)
if model == "V2" or model == "v2":
    gAXd, gVXd, gAq, gVq, gAl, gVl = 0.0, 1.0, 0.0, 0.1, 0.0, 0.01
    WY1 = totalWidthVector(float(MY1),float(MXd),0.1,1.0,0.01)

username =  pwd.getpwuid(os.getuid())[0]
wdir = "/tmp/%s/"%username
odir = os.getcwd()
sdir = odir + "/results/med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s"%(str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut)
infile = open("paths_and_parameters.dat","r")
for line in infile:
    if "MG5_path" in line: mg5dir = line.split("=")[1].split("\n")[0]
    if "lhapdf_path" in line: os.environ["LD_LIBRARY_PATH"] = line.split("=")[1].split("\n")[0]
infile.close()

os.chdir(wdir)
mg5file = open('mg5.txt', 'w')
mg5file.write('set low_mem_multicore_nlo_generation True \n')
mg5file.write('import model DMsimp_s_spin1 \n')
mg5file.write('generate p p > xd xd~ j [QCD] \n')
mg5file.write('add process p p > xd xd~ j j [QCD] \n')
mg5file.write('output med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s_repeat%s \n'%(str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut,repeat_index))
mg5file.write('launch \n')
mg5file.write('set my1 %s \n'%str(MY1))
mg5file.write('set mxd %s \n'%str(MXd))
mg5file.write('set wy1 %s \n'%str(WY1))
mg5file.write('set gAXd %s \n'%str(gAXd))
mg5file.write('set gVXd %s \n'%str(gVXd))
mg5file.write('set gAd11 %s \n'%str(gAq))
mg5file.write('set gAd22 %s \n'%str(gAq))
mg5file.write('set gAd33 %s \n'%str(gAq))
mg5file.write('set gAu11 %s \n'%str(gAq))
mg5file.write('set gAu22 %s \n'%str(gAq))
mg5file.write('set gAu33 %s \n'%str(gAq))
mg5file.write('set gVd11 %s \n'%str(gAq))
mg5file.write('set gVd22 %s \n'%str(gVq))
mg5file.write('set gVd33 %s \n'%str(gVq))
mg5file.write('set gVu11 %s \n'%str(gVq))
mg5file.write('set gVu22 %s \n'%str(gVq))
mg5file.write('set gVu33 %s \n'%str(gVq))
mg5file.write('set gAl11 %s \n'%str(gAl))
mg5file.write('set gAl22 %s \n'%str(gAl))
mg5file.write('set gAl33 %s \n'%str(gAl))
mg5file.write('set gVl11 %s \n'%str(gVl))
mg5file.write('set gVl22 %s \n'%str(gVl))
mg5file.write('set gVl33 %s \n'%str(gVl))
mg5file.write('set parton_shower PYTHIA8 \n')
mg5file.write('set ickkw 3 \n')
mg5file.write('set iseed 1234 \n')
mg5file.write('set ebeam1 %s \n'%ebeam)
mg5file.write('set ebeam2 %s \n'%ebeam)
mg5file.write('set nevents %s \n'%nevents)
mg5file.write('set pdlabel lhapdf \n')
mg5file.write('set lhaid 260000 \n')
mg5file.write('set ptj %s \n'%ptj)
mg5file.write('set Qcut %s \n'%Qcut)
mg5file.write('set njmax 1 \n')
mg5file.close()

os.system("%s/bin/mg5_aMC mg5.txt"%mg5dir)
os.system("cat /tmp/%s/med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s_repeat%s/Cards/param_card.dat"%(username,str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut,repeat_index))
os.system("cat /tmp/%s/med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s_repeat%s/Cards/run_card.dat"%(username,str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut,repeat_index))
os.system("cp /tmp/%s/med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s_repeat%s/Events/run_01/*.hepmc.gz %s/med%s_dm%s_%s_beam%s_n%s_ptj%s_qcut%s_repeat%s.hepmc.gz"%(username,str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut,repeat_index,sdir,str(MY1),str(MXd),model,ebeam,nevents,ptj,Qcut,repeat_index))
os.chdir(odir)
