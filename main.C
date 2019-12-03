#include <iostream>
#include "stdlib.h"
#include "LHChain.h"
#include "TFile.h"
#include "common.h"
using namespace std;
const int timeshift=788918400;
int main(int argc,char* argv[]){
   if(argc<3){
      printf("Usage %s <runlist> <outname> <iTel> <First> <Last> <MaxEntries> <iSiPM_start> <iSiPM_end>\n",argv[0]);
      return 0;
   }
   WFCTAEvent::npetrigger=100;
   WFCTAEvent::nfiretrigger=3;

   char* runlist=argv[1];
   char* outname=argv[2];
   char firstline[300];
   char lastline[300];
   int nline=CommonTools::GetFirstLastLine(Form("/afs/ihep.ac.cn/users/c/chenqh/x/WFCTA/Noise/%s",runlist),firstline,lastline);
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
   const int nnsbin=200;
   double nsbin[nnsbin+1];
   double nsbin2[nnsbin+1];
   for(int ii=0;ii<=nnsbin;ii++){
      nsbin[ii]=1.+(200.-1.)/nnsbin*ii;
      nsbin2[ii]=1.+(200.*13./19.-1.)/nnsbin*ii;
   }
   const int nbsbin=250;
   double bsbin[nbsbin+1];
   for(int ii=0;ii<=nbsbin;ii++){
      bsbin[ii]=1.+(400.-1.)/nbsbin*ii;
   }

   int itel=(argc>3)?atoi(argv[3]):-1;
   int first=(argc>4)?atoi(argv[4]):0;
   int last=(argc>5)?atoi(argv[5]):(nline-1);
   int maxentry=(argc>6)?atoi(argv[6]):-1;
   int isipm0=(argc>7)?atoi(argv[7]):-1;
   int isipm1=(argc>8)?atoi(argv[8]):-1;

   TH2F* hnoise[1024];
   TH2F* hnoise2[1024];
   for(int ii=0;ii<1024;ii++){
      hnoise[ii]=0;
      hnoise2[ii]=0;
      if((isipm0>=0&&isipm0<=isipm1)&&(ii<isipm0||ii>isipm1)) continue;
      hnoise[ii]=new TH2F(Form("hnoise_sipm%d",ii),Form("SiPM%d;Time;Noise [pe]",ii),nxbin,xbins,nnsbin,nsbin);
      hnoise[ii]->GetXaxis()->SetTimeDisplay(1);
      hnoise[ii]->GetXaxis()->SetNdivisions(-203);
      hnoise[ii]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
      hnoise2[ii]=new TH2F(Form("hnoise_winsum_sipm%d",ii),Form("SiPM%d;Time;winsum of Noise [pe]",ii),nxbin,xbins,nnsbin,nsbin2);
      hnoise2[ii]->GetXaxis()->SetTimeDisplay(1);
      hnoise2[ii]->GetXaxis()->SetNdivisions(-203);
      hnoise2[ii]->GetXaxis()->SetTimeFormat("%Mm/%Hh/%d/%m%F1970-01-01 00:00:00s0");
   }

   TH2F* hbaseline[1024];
   for(int ii=0;ii<1024;ii++){
      hbaseline[ii]=0;
      if((isipm0>=0&&isipm0<=isipm1)&&(ii<isipm0||ii>isipm1)) continue;
      hbaseline[ii]=new TH2F(Form("hbaseline_sipm%d",ii),Form("SiPM%d;Time;High Gain BaseLine [pe]",ii),nxbin,xbins,nbsbin,bsbin);
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

   WCamera::SetSiPMMAP();
   TH2F* mapnoise=new TH2F("mapnoise",";SiPM Index;Noise [pe]",1024,-0.5,1023.5,nnsbin,nsbin);
   TH2F* mapnoise2=new TH2F("mapnoise_winsum",";SiPM Index;Noise [pe]",1024,-0.5,1023.5,nnsbin,nsbin2);
   TH2F* mapbaseline=new TH2F("mapbaseline",";SiPM Index;High Gain BaseLine [pe]",1024,-0.5,1023.5,nbsbin,bsbin);

   TH2F* noise_vs_baseline[1024];
   TH2F* noise2_vs_baseline[1024];
   for(int ii=0;ii<1024;ii++){
      noise_vs_baseline[ii]=0;
      noise2_vs_baseline[ii]=0;
      if((isipm0>=0&&isipm0<=isipm1)&&(ii<isipm0||ii>isipm1)) continue;
      noise_vs_baseline[ii]=new TH2F(Form("noise_vs_baseline_sipm%d",ii),Form("SiPM%d;High Gain BaseLine [pe];Noise [pe]",ii),nbsbin,bsbin,nnsbin,nsbin);
      noise2_vs_baseline[ii]=new TH2F(Form("noise_winsum_vs_baseline_sipm%d",ii),Form("SiPM%d;High Gain BaseLine [pe];Noise [pe]",ii),nbsbin,bsbin,nnsbin,nsbin2);
   }

   TH1F* hsingle=new TH1F(Form("single_thresh_sipm%d",0),";Time;Single Threshold [ADC]",nxbin,xbins);
   TH1F* hrecord=new TH1F(Form("record_thresh_sipm%d",0),";Time;Record Threshold [ADC]",nxbin,xbins);

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

   TChain statuschain("Status");
   int iline=0;
   char buff[500];
   ifstream fin;
   fin.open(Form("/afs/ihep.ac.cn/users/c/chenqh/x/WFCTA/Noise/%s",runlist),std::ios::in);
   fin.getline(buff,500);
   iline++;
   while(fin.good()){
      char statusfile[500];
      CommonTools::GetStatusFile(statusfile,buff);
      if(last>=first&&first>=0){
         if(iline-1>=first&&iline-1<=last){
            statuschain.Add(statusfile);
            printf("Status Tree Adding %d: %s\n",iline,statusfile);
         }
         else if(iline-1>last) break;
      }
      else{
         statuschain.Add(statusfile);
         printf("Status Tree Adding %d: %s\n",iline,statusfile);
      }
      fin.getline(buff,500);
      iline++;
   }
   long status_readback_Time=0;
   short single_thresh[1024]={0};
   short record_thresh[1024]={0};
   statuschain.SetBranchAddress("status_readback_Time",&status_readback_Time);
   statuschain.SetBranchAddress("single_thresh",single_thresh);
   statuschain.SetBranchAddress("record_thresh",record_thresh);
   //statuschain.GetEntry(0);
   //printf("Time=%ld record=%d\n",status_readback_Time,(int)record_thresh[0]);
   //return 1;

   LHChain chain;
   chain.AddFromFile(Form("/afs/ihep.ac.cn/users/c/chenqh/x/WFCTA/Noise/%s",runlist),first,last);
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

      //ignore events without status data
      int pstatustime=status_readback_Time;
      while(fabs(time-status_readback_Time)>18){
         statuschain.GetEntry();
         if(status_readback_Time==pstatustime) break;
         else pstatustime=status_readback_Time;
         if(status_readback_Time>time) break;
      }
      bool findstatus=fabs(time-status_readback_Time)<=18;
      if(!findstatus) continue;
      //printf("entry=%d time=%lf status_time=%ld findstatus=%d\n",entry,time,status_readback_Time,findstatus);

      int itbin=hsingle->GetXaxis()->FindBin(status_readback_Time*1.);
      if(single_thresh[0]>hsingle->GetBinContent(itbin)) hsingle->SetBinContent(itbin,single_thresh[0]);
      if(record_thresh[0]>hrecord->GetBinContent(itbin)) hrecord->SetBinContent(itbin,record_thresh[0]);

      int size=pev->iSiPM.size();
      for(int ii=0;ii<size;ii++){
         double nnoise=pev->GetContent(ii,0,3,true);
         double nnoise2=pev->winsum.at(ii)/WFCTAMCEvent::fAmpHig;
         double nbase=pev->BaseH.at(ii)/WFCTAMCEvent::fAmpHig;
         if(hnoise[pev->iSiPM.at(ii)]) hnoise[pev->iSiPM.at(ii)]->Fill(time,nnoise);
         if(mapnoise) mapnoise->Fill(pev->iSiPM.at(ii),nnoise);
         if(noise_vs_baseline[pev->iSiPM.at(ii)]) noise_vs_baseline[pev->iSiPM.at(ii)]->Fill(nbase,nnoise);
         if(record_thresh[pev->iSiPM.at(ii)]<5){
         if(hnoise2[pev->iSiPM.at(ii)]) hnoise2[pev->iSiPM.at(ii)]->Fill(time,nnoise2);
         if(mapnoise2) mapnoise2->Fill(pev->iSiPM.at(ii),nnoise2);
         if(noise2_vs_baseline[pev->iSiPM.at(ii)]) noise2_vs_baseline[pev->iSiPM.at(ii)]->Fill(nbase,nnoise2);
         }
      }
   }

   TFile* fout=TFile::Open(outname,"RECREATE");
   if(hrawrate) hrawrate->Write();
   if(hselrate) hselrate->Write();
   if(mapnoise) mapnoise->Write();
   if(mapnoise2) mapnoise2->Write();
   if(mapbaseline) mapbaseline->Write();
   for(int ii=0;ii<1024;ii++){
      if(hnoise[ii]) hnoise[ii]->Write();
      if(hnoise2[ii]) hnoise2[ii]->Write();
      if(hbaseline[ii]) hbaseline[ii]->Write();
      if(noise_vs_baseline[ii]) noise_vs_baseline[ii]->Write();
      if(noise2_vs_baseline[ii]) noise2_vs_baseline[ii]->Write();
   }
   if(hsingle) hsingle->Write();
   if(hrecord) hrecord->Write();
   fout->Close();

   return 1;
}
