{
   gStyle->SetOptLogy(0);
   gStyle->SetOptFit(1111);
   gStyle->SetMarkerStyle(20);
   gStyle->SetMarkerColor(4);
   gStyle->SetMarkerSize(1.0);
   gStyle->SetLineColor(4);
   gStyle->SetLineWidth(2);

   TFile* fin=TFile::Open("/eos/user/c/chenqh/x/noise/2/0hh.root");

   int isipm=1;
   TH2F* hist=(TH2F*)fin->Get(Form("noise_vs_baseline_sipm%d",isipm));
   //hist->Draw("colz");

   TGraphErrors* gr=new TGraphErrors();
   TGraphErrors* gr2=new TGraphErrors();
   TGraphErrors* gr3=new TGraphErrors();
   for(int ibin=1;ibin<hist->GetNbinsX();ibin++){
      TH1D* hy=hist->ProjectionY(Form("bin%d",ibin),ibin,ibin);
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
      if(fabs(epeak/peak)>0.3) {delete hy; continue;}
      if(fabs(esigm/sigm)>0.3) {delete hy; continue;}
      int np=gr->GetN();
      gr->SetPoint(np,hist->GetXaxis()->GetBinCenter(ibin),peak);
      gr->SetPointError(np,0,epeak);
      np=gr2->GetN();
      gr2->SetPoint(np,hist->GetXaxis()->GetBinCenter(ibin),sigm);
      gr2->SetPointError(np,0,esigm);
      np=gr3->GetN();
      gr3->SetPoint(np,hist->GetXaxis()->GetBinCenter(ibin),peak+3*sigm);
      gr3->SetPointError(np,0,sqrt(pow(epeak,2)+pow(3*esigm,2)));
   }
   TCanvas* cc=new TCanvas();
   cc->Divide(3,1);
   cc->cd(1);
   gr->Draw("ap");
   gr->SetTitle(";High Gain Baseline [pe];Noise Peak Value [pe]");
   cc->cd(2);
   gr2->Draw("ap");
   gr2->SetTitle(";High Gain Baseline [pe];Noise Sigma Value [pe]");
   cc->cd(3);
   gr3->Fit("pol1","QS+");
   gr3->Draw("ap");
   gr3->SetTitle(";High Gain Baseline [pe];Noise Peak+3*Sigma Value [pe]");
}
