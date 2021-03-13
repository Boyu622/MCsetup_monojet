import os,sys,pwd
from exclCalc import *

if len(sys.argv)<12:
    print "Please pass arguments following: python mg5nlo_launch_single.py {mDM} {Mmed} {med_type} {gDM} {gq} {gl} {model_tag} {Ebeam_single} {nevents} {xqcut} {repeat}"
    sys.exit()
else:
    MXd = sys.argv[1]
    MY1 = sys.argv[2]
    med_type = sys.argv[3]
    gDM = sys.argv[4]
    gq = sys.argv[5]
    gl = sys.argv[6]
    model = sys.argv[7]
    ebeam = sys.argv[8]
    nevents = sys.argv[9]
    xqcut = sys.argv[10]
    repeat_index = sys.argv[11]
    
if med_type == "Axial" or med_type == "axial":
    gVXd, gVq, gVl = 0,0,0
    gAXd, gAq, gAl = float(gDM), float(gq), float(gl)
    WY1 = totalWidthAxial(float(MY1),float(MXd),gAq,gAXd,gAl)
if med_type == "Vector" or med_type == "vector":
    gAXd, gAq, gAl = 0,0,0
    gVXd, gVq, gVl = float(gDM), float(gq), float(gl)
    WY1 = totalWidthVector(float(MY1),float(MXd),gVq,gVXd,gVl)

username =  pwd.getpwuid(os.getuid())[0]
wdir = "/tmp/%s/"%username
odir = os.getcwd()
sdir = odir + "/results/lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s"%(str(MY1),str(MXd),model,ebeam,nevents,xqcut)
os.system("mkdir -p %s"%sdir)
infile = open("launch_many_parameters.dat","r")
for line in infile:
    if "MG5_path" in line: mg5dir = line.split("=")[1].split("\n")[0]
    if "lhapdf_path" in line: os.environ["LD_LIBRARY_PATH"] = line.split("=")[1].split("\n")[0].strip(" ")
infile.close()

os.chdir(wdir)
mg5file = open('mg5.txt', 'w')

mg5file.write('set default_unset_couplings 99 \n')
mg5file.write('set group_subprocesses Auto \n')
mg5file.write('set ignore_six_quark_processes False \n')
mg5file.write('set loop_optimized_output True \n')
mg5file.write('set loop_color_flows False \n')
mg5file.write('set gauge unitary \n')
mg5file.write('set complex_mass_scheme False \n')
mg5file.write('set max_npoint_for_channel 0 \n')

mg5file.write('import model DMsimp_s_spin1 \n')
mg5file.write('generate p p > xd xd~ j @0 \n')
mg5file.write('add process p p > xd xd~ j j @1 \n')
mg5file.write('output lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s_repeat%s \n'%(str(MY1),str(MXd),model,ebeam,nevents,xqcut,repeat_index))
mg5file.write('launch \n')
mg5file.write('shower=Pythia8 \n')
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
mg5file.write('set ickkw 1 \n')
mg5file.write('set iseed 222 \n')
mg5file.write('set ebeam1 %s \n'%ebeam)
mg5file.write('set ebeam2 %s \n'%ebeam)
mg5file.write('set nevents %s \n'%nevents)
mg5file.write('set pdlabel nn23lo1 \n')
mg5file.write('set lhaid 230000 \n')
mg5file.write('set xptj 150 \n')
mg5file.write('set xqcut %s \n'%xqcut)
mg5file.write('set use_syst False \n')
mg5file.close()

os.system("%s/bin/mg5_aMC mg5.txt"%mg5dir)
os.system("cat /tmp/%s/lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s_repeat%s/Cards/param_card.dat"%(username,str(MY1),str(MXd),model,ebeam,nevents,xqcut,repeat_index))
os.system("cat /tmp/%s/lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s_repeat%s/Cards/run_card.dat"%(username,str(MY1),str(MXd),model,ebeam,nevents,xqcut,repeat_index))
os.system("cp /tmp/%s/lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s_repeat%s/Events/run_01/*.hepmc.gz %s/lo_med%s_dm%s_%s_beam%s_n%s_xqcut%s_repeat%s.hepmc.gz"%(username,str(MY1),str(MXd),model,ebeam,nevents,xqcut,repeat_index,sdir,str(MY1),str(MXd),model,ebeam,nevents,xqcut,repeat_index))
os.chdir(odir)
