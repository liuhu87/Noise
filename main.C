#include <iostream>
#include "stdlib.h"
#include "LHChain.h"
#include "TFile.h"
using namespace std;
const int timeshift=788918400;
int main(int argc,char* argv[]){
   if(argc<3){
      printf("Usage %s <runlist> <outname> <iTel> <First> <Last> <MaxEntries>\n",argv[0]);
      return 0;
   }
   WFCTAEvent::npetrigger=100;
   WFCTAEvent::nfiretrigger=3;

   char* runlist=argv[1];
   char* outname=argv[2];
   char firstline[300];
   char lastline[300];
   int nline=CommonTools::GetFirstLastLine(Form("/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/Noise/%s",runlist),firstline,lastline);
   int timefirst=CommonTools::GetTimeFromFileName(firstline,59,12)-600;
   int timelast=CommonTools::GetTimeFromFileName(lastline,59,12)+600;
   printf("firstline=%s\n",firstline);
   printf("lastline =%s\n",lastline);
   printf("nline=%d timefirst=%d timelast=%d\n",nline,timefirst,timelast);
   double xbins[100000];
   int nxbin=CommonTools::GetBins(timefirst,timelast,60.,xbins);
   printf("Total %d time bins\n",nxbin);
   if(nxbin==100000-1){
      printf("BinError,Please Increase BinWidth, Exiting...\n");
      return 0;
   }

   int itel=(argc>3)?atoi(argv[3]):-1;
   int first=(argc>4)?atoi(argv[4]):0;
   int last=(argc>5)?atoi(argv[5]):(nline-1);
   int maxentry=(argc>6)?atoi(argv[6]):-1;

   TH2F* hnoise[1024];
   for(int ii=0;ii<1024;ii++){
   hnoise[ii]=new TH2F(Form("hnoise_sipm%d",ii),Form("SiPM%d;Time;Noise [pe]",ii),nxbin,xbins,200,1.,300.);
   hnoise[ii]->GetXaxis()->SetTimeDisplay(1);
   hnoise[ii]->GetXaxis()->SetNdivisions(-203);
   hnoise[ii]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
   }

   TH2F* hbaseline[1024];
   for(int ii=0;ii<1024;ii++){
   hbaseline[ii]=new TH2F(Form("hbaseline_sipm%d",ii),Form("SiPM%d;Time;High Gain BaseLine [pe]",ii),nxbin,xbins,200,1.,300.);
   hbaseline[ii]->GetXaxis()->SetTimeDisplay(1);
   hbaseline[ii]->GetXaxis()->SetNdivisions(-203);
   hbaseline[ii]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
   }

   TH1F* hrawrate=new TH1F("hrawrate",";Time;Events",nxbin,xbins);
   hrawrate->GetXaxis()->SetTimeDisplay(1);
   hrawrate->GetXaxis()->SetNdivisions(-203);
   hrawrate->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
   TH1F* hselrate=new TH1F("hselrate",";Time;Events",nxbin,xbins);
   hselrate->GetXaxis()->SetTimeDisplay(1);
   hselrate->GetXaxis()->SetNdivisions(-203);
   hselrate->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");

   //WCamera::SetSiPMMAP();
   TH2F* mapnoise=0;//new TH2F("mapnoise",";SiPM Index;Noise [pe]",1024,-0.5,1023.5,200,1.,300.);
   TH2F* mapbaseline=0;//new TH2F("mapbaseline",";SiPM Index;High Gain BaseLine [pe]",1024,-0.5,1023.5,200,1.,300.);

   TH2F* noise_vs_baseline[1024];
   for(int ii=0;ii<1024;ii++){
      noise_vs_baseline[ii]=0;
      //noise_vs_baseline[ii]=new TH2F(Form("noise_vs_baseline_sipm%d",ii),Form("SiPM%d;High Gain BaseLine [pe];Noise [pe]",ii),200,1.,300.,200,1.,300.);
   }

   if(false){
      for(int ibin=1;ibin<=nxbin;ibin++){
         hnoise[0]->SetBinContent(ibin,100,ibin*1.);
         hbaseline[0]->SetBinContent(ibin,100,ibin*1.);
      }
      TFile* fout=TFile::Open(outname,"RECREATE");
      hnoise[0]->Write();
      hbaseline[0]->Write();
      fout->Close();
      return 1;
   }

   LHChain chain;
   chain.AddFromFile(Form("/afs/ihep.ac.cn/users/h/hliu/Documents/Analysis/Noise/%s",runlist),first,last);
   if(maxentry<0) maxentry=chain.GetEntries();
   for(int entry=0;entry<maxentry;entry++){
      if((entry%1000)==0) printf("Processing %d of %d entries\n",entry,maxentry);
      WFCTAEvent* pev=chain.GetEvent();
      double time=pev->rabbitTime+pev->rabbittime*20*1.0e-9;
      if((itel>0)&&(itel!=pev->iTel)) continue;
      if(hrawrate) hrawrate->Fill(time);
      if(pev->PassClean(pev->iTel,3)){
         if(hselrate) hselrate->Fill(time);
      }
      for(int ii=0;ii<pev->iSiPM.size();ii++){
         double nbase=pev->BaseH.at(ii)/WFCTAMCEvent::fAmpHig;
         if(hbaseline[pev->iSiPM.at(ii)]) hbaseline[pev->iSiPM.at(ii)]->Fill(time,nbase);
         if(mapbaseline) mapbaseline->Fill(pev->iSiPM.at(ii),nbase);
      }
      if(!pev->IsNoise(5,150,4,3.5)) continue;
      int size=pev->iSiPM.size();
      for(int ii=0;ii<size;ii++){
         double nnoise=pev->GetContent(ii,0,3,true);
         double nbase=pev->BaseH.at(ii)/WFCTAMCEvent::fAmpHig;
         if(hnoise[pev->iSiPM.at(ii)]) hnoise[pev->iSiPM.at(ii)]->Fill(time,nnoise);
         if(mapnoise) mapnoise->Fill(pev->iSiPM.at(ii),nnoise);
         if(noise_vs_baseline[pev->iSiPM.at(ii)]) noise_vs_baseline[pev->iSiPM.at(ii)]->Fill(nbase,nnoise);
      }
   }

   TFile* fout=TFile::Open(outname,"RECREATE");
   if(hrawrate) hrawrate->Write();
   if(hselrate) hselrate->Write();
   if(mapnoise) mapnoise->Write("mapnoise");
   if(mapbaseline) mapbaseline->Write("mapbase");
   for(int ii=0;ii<1024;ii++){
   hnoise[ii]->Write();
   hbaseline[ii]->Write();
   if(noise_vs_baseline[ii]) noise_vs_baseline[ii]->Write();
   }
   fout->Close();

   return 1;
}
