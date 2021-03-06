import os,sys

if len(sys.argv)<2: 
  print "please provide directory that contains all hepmc files you want to combine"
  sys.exit()

os.system("cp rivet_monojet.cc %s/sample.cc"%sys.argv[1])
os.system("cp rivet_monojet.plot %s/sample.plot"%sys.argv[1])

os.chdir(sys.argv[1])

os.system("rivet-mkanalysis rivet_monojet")
os.system("mv sample.cc rivet_monojet.cc")
os.system("mv sample.plot rivet_monojet.plot")
os.system("rivet-buildplugin rivet_monojet.cc")

filestr = " "
for filename in os.listdir("."):
  if "hepmc" in filename: filestr = filestr + filename + " "

os.system("rivet --analysis=rivet_monojet %s"%filestr)
os.system("rivet-mkhtml --pwd Rivet.yoda:'Title=PYTHIA 8.2'")

files = ["EM1.dat","EM2.dat","EM3.dat","EM4.dat","EM5.dat","EM6.dat","EM7.dat","EM8.dat","EM9.dat","EM10.dat"]
info = []
for i, fname in enumerate(files):
  infile = open("./rivet-plots/rivet_monojet/" + fname, "r")
  for j, line in enumerate(infile):
    if j == 14: info.append(line)
    if i == 0 and "# BEGIN HISTO1D" in line: 
      header = line.split(" ")[3].split("EM1")[0]
      merginfo = line.split(" ")[3].split("EM1")[1]
  infile.close()
outfile = open("./rivet-plots/rivet_monojet/" + "MET.dat", "w")
outfile.write("# BEGIN PLOT" + "\n")
outfile.write("LegendOnly=" + header + "MET" + merginfo + "\n")
outfile.write("XLabel=MET [GeV]" + "\n")
outfile.write("LegendAlign=r" + "\n")
outfile.write("YLabel=Events/GeV" + "\n")
outfile.write("DrawOnly=" + header + "MET" + merginfo + "\n")
outfile.write("Title=MET distribution" + "\n")
outfile.write("Legend=1" + "\n")
outfile.write("LogY=1" + "\n")
outfile.write("# END PLOT" + "\n")
outfile.write("\n")
outfile.write("# BEGIN HISTO1D " + header + "MET" + merginfo + "\n")
outfile.write("ErrorBars=1" + "\n")
outfile.write("LineColor=red" + "\n")
outfile.write("LineStyle=solid" + "\n")
outfile.write("Path=" + header + "MET \n")
outfile.write("Title=Rivet" + "\n")
outfile.write("# xlow	 xhigh	 val	 errminus	 errplus" + "\n")
for ele in info:
  outfile.write(ele)
outfile.write("# END HISTO1D" + "\n")

print "you can make MET histogram by 'make-plots --png ./rivet-plots/rivet_monojet/MET.dat' at %s"%os.getcwd()
