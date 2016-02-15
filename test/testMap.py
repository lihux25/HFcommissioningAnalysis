from ROOT import *
gROOT.SetBatch(True)
from optparse import OptionParser

gROOT.ProcessLine(".L ~/tdrstyle.C")
gROOT.ProcessLine("setTDRStyle()")

parser = OptionParser()
parser.add_option("-r", "--run", dest="runNumber",
                  help="runNumber", metavar="FILE")
(options, args) = parser.parse_args()

inFileName = "HFanalysisTree_000{0}.root".format(options.runNumber)
inFile = TFile(inFileName)

can = TCanvas("can","can",600,900)
can.Divide(4,6)
prof = []
histo = []
leg = []

anodeDiff = TH1F("anodeDiff","anodeDiff",20,-.5,.5)

for i in range(4): # i: eta
    for j in range(6): # j: phi
        can.cd(j*4+i+1)
        if inFile.Get("Qpulse_ieta{0}_iphi{1}_depth1".format(i+1,j+1)) != None :
            histo.append( TH2F( inFile.Get("Qpulse_ieta{0}_iphi{1}_depth1".format(i+1,j+1)) ) )
            prof.append( histo[-1].ProfileX("Qpulse_ieta{0}_iphi{1}_depth1_profile".format(i+1,j+1),1,-1,"S") )
            prof[-1].SetLineColor(4)
            prof[-1].SetMarkerColor(4)
            prof[-1].SetLineWidth(2)
        else  :
            histo.append(0)
            prof.append(0)

        if inFile.Get("Qpulse_ieta{0}_iphi{1}_depth2".format(i+1,j+1) ) != None :
            histo.append( TH2F( inFile.Get("Qpulse_ieta{0}_iphi{1}_depth2".format(i+1,j+1) ) ) )
            prof.append( histo[-1].ProfileX("Qpulse_ieta{0}_iphi{1}_depth2_profile".format(i+1,j+1),1,-1,"S") )
            prof[-1].SetLineColor(2)
            prof[-1].SetMarkerColor(2)
            prof[-1].SetLineWidth(2)
        else : 
            histo.append(None)
            prof.append(None)

        leg.append( TLegend(.6,.75,.9,.9) )
        leg[-1].SetHeader("Qpulse_ieta{0}_iphi{1}".format(i+1,j+1))
        leg[-1].Draw()

        if prof[-2] == None and prof[-1] == None : continue
        if prof[-2] == None :
            prof[-1].Draw()
            prof[-1].SetTitle("Qpulse_ieta{0}_iphi{1}".format(i+1,j+1))
        elif prof[-1] == None :
            prof[-2].Draw()
            prof[-2].SetTitle("Qpulse_ieta{0}_iphi{1}".format(i+1,j+1))
        else :
            prof[-2].Draw()
            prof[-1].Draw("SAME")
            prof[-2].SetTitle("Qpulse_ieta{0}_iphi{1}".format(i+1,j+1))
            prof[-2].GetYaxis().SetRangeUser( 0 , max( prof[-2].GetMaximum() , prof[-1].GetMaximum() )*1.3 )

can.SaveAs("testMap_{0}.png".format(options.runNumber))
can.SaveAs("testMap_{0}.pdf".format(options.runNumber))
