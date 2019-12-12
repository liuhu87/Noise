{
   gStyle->SetOptLogy(0);
   gStyle->SetOptStat(0);
   //gStyle->SetOptFit(1111);
   gStyle->SetMarkerStyle(20);
   gStyle->SetMarkerColor(4);
   gStyle->SetMarkerSize(1.0);
   gStyle->SetLineColor(4);
   gStyle->SetLineWidth(2);

   TF1* f1=new TF1("f1","[0]+[1]*x",0,1000);
   f1->SetParameters(70-0.5*34,0.5);
   f1->SetLineColor(1);
   f1->SetLineWidth(3);

   int iTel=3;
   double nsigma=4.5;
   TFile* fin=TFile::Open(Form("/eos/user/h/hliu/Noise/all_tel%d.root",iTel));
   int indexi=-1;

   const int nsipm=10;
   MSpline* spls[nsipm];
   for(int ii=0;ii<nsipm;ii++) spls[ii]=0;
   for(int ii=0;ii<nsipm;ii++){
      int isipm=ii;
      if(indexi>=0&&indexi!=isipm) continue;
      TH2F* hist=(TH2F*)fin->Get(Form("noise_winsum_vs_baseline_sipm%d",isipm));
      hist->Rebin2D(4,1);
      hist->GetYaxis()->SetRangeUser(0.,98.);
      if(indexi>=0){
         hist->Draw("colz");
         f1->Draw("same");
      }

      TGraphErrors* gr=new TGraphErrors();
      TGraphErrors* gr2=new TGraphErrors();
      TGraphErrors* gr3=new TGraphErrors();
      for(int ibin=1;ibin<hist->GetNbinsX();ibin++){
         TH1D* hy=hist->ProjectionY(Form("bin%d",ibin),ibin,ibin);
         for(int ibin=hy->GetXaxis()->FindBin(99.);ibin<=hy->GetNbinsX();ibin++) hy->SetBinContent(ibin,0);
         if(hy->Integral()<100) {delete hy; continue;}
         //hy->Draw();
         //if(gPad) gPad->Update();
         //if(gPad) gPad->WaitPrimitive();
         
         hy->Fit("gaus","RQS0","",0.,200.);
         TF1* fun=hy->GetFunction("gaus");
         if(!fun) {delete hy; continue;}
         double peak=fun->GetParameter(1);
         double epeak=fun->GetParError(1);
         double sigm=fun->GetParameter(2);
         double esigm=fun->GetParError(2);

         //double peak=hy->GetMean();
         //double epeak=hy->GetMeanError();
         //double sigm=hy->GetRMS();
         //double esigm=hy->GetRMSError();

         if(fabs(epeak/peak)>0.6) {delete hy; continue;}
         if(fabs(esigm/sigm)>0.6) {delete hy; continue;}
         int np=gr->GetN();
         gr->SetPoint(np,hist->GetXaxis()->GetBinCenter(ibin),peak);
         gr->SetPointError(np,0,epeak);
         np=gr2->GetN();
         gr2->SetPoint(np,hist->GetXaxis()->GetBinCenter(ibin),sigm);
         gr2->SetPointError(np,0,esigm);
         np=gr3->GetN();
         //gr3->SetPoint(np,hist->GetXaxis()->GetBinCenter(ibin),peak+(nsigma-2.01)/0.46*sigm);
         //gr3->SetPointError(np,0,sqrt(pow(epeak,2)+pow((nsigma-2.01)/0.46*esigm,2)));
         gr3->SetPoint(np,hist->GetXaxis()->GetBinCenter(ibin),peak+(nsigma-2.01)/0.46*sigm);
         gr3->SetPointError(np,0,sqrt(pow(epeak,2)+pow((nsigma-2.01)/0.46*esigm,2)));
         delete hy;
      }

      int nknots=4;
      double xknots[4]={30.,50.,iTel!=3?120.:70.,iTel!=3?250.:100.};
      MSpline* spl=new MSpline(nknots, xknots, false, false, MSpline::kFlat, MSpline::kFlat);
      spl->Fit(gr3);
      spls[ii]=spl;

      if(indexi>=0){
         TCanvas* cc=new TCanvas();
         cc->Divide(3,1);
         cc->cd(1);
         gr->Draw("ap");
         gr->SetTitle(";High Gain Baseline [pe];Noise Peak Value [pe]");
         cc->cd(2);
         gr2->Draw("ap");
         gr2->SetTitle(";High Gain Baseline [pe];Noise Sigma Value [pe]");
         cc->cd(3);
         //gr3->Fit("pol1","QS+");
         gr3->Draw("ap");
         gr3->SetTitle(";High Gain Baseline [pe];Noise Peak+3*Sigma Value [pe]");
         f1->Draw("same");
         spl->Draw("l", 2, 2, 1000, 0., 500.);
         printf("baseline=300. peak=%lf sigma=%lf\n",spl->Eval(300.),spl->Eval(300.));
      }
   }

   if(indexi<0){
      TCanvas* cc=new TCanvas();
      int np=0;
      for(int ii=0;ii<nsipm;ii++){
        if(!spls[ii]) continue;
        spls[ii]->Draw(np==0?"al":"l", 2, 2, 1000, 0., 500.);
        np++;
      }
      f1->Draw("same");
   }
}
