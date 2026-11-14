#include "../lhcbStyle.C"

using namespace RooFit;

void sigandbg(){

	lhcbStyle();

    TFile *f = new TFile("histogram_file.root");

    TH1F *h1 = (TH1F*)f->Get("h1");
    TH1F *h2 = (TH1F*)f->Get("h2");
    h1->SetLineColor(kRed);
    h2->SetLineColor(kBlue);

//     RooRealVar x("x","M(J/#psi #pi^{+}#pi^{-}) [MeV]",3800,3950);
// 
//     RooDataHist data1("data1","dataset1",x,h1);
//     RooDataHist data2("data2","dataset2",x,h2);
// 
// 
//     RooRealVar mean1("mean1","mean",3872,3860,3885);
//     RooRealVar sigma1("sigma1","sigma",3.30,1,20);
// 
//     RooRealVar alphaL1("alphaL1","alphaL",1.5,0.1,5);
//     RooRealVar nL1("nL1","nL",5,0.1,10);
// 
//     RooRealVar alphaR1("alphaR1","alphaR",2.0,0.1,5);
//     RooRealVar nR1("nR1","nR",5,0.1,10);
// 
//     RooCrystalBall signal1("signal1","DSCB Signal",
//                           x,
//                           mean1,
//                           sigma1,
//                           alphaL1,
//                           nL1,
//                           alphaR1,
//                           nR1);
// 
//     RooRealVar mean2("mean2","mean",3872,3860,3885);
//     RooRealVar sigma2("sigma2","sigma",3.30,1,20);
// 
//     RooRealVar alphaL2("alphaL2","alphaL",1.5,0.1,5);
//     RooRealVar nL2("nL","nL",5,0.1,10);
// 
//     RooRealVar alphaR2("alphaR2","alphaR",2.0,0.1,5);
//     RooRealVar nR2("nR2","nR",5,0.1,10);
// 
//     RooCrystalBall signal2("signal","DSCB Signal",
//                           x,
//                           mean2,
//                           sigma2,
//                           alphaL2,
//                           nL2,
//                           alphaR2,
//                           nR2);
// 
//     RooRealVar x0("x0","x0",0,-1,1);
//     RooRealVar x1("x1","x1",0,-1,1);
// 
//     RooChebychev background("background","background",
//                             x,
//                             RooArgList(x0,x1));
// 
//     RooRealVar nsig1("nsig1","signal yield",1000,0,1e6);
//     RooRealVar nbkg1("nbkg1","background yield",1000,0,1e6);
// 
//     RooAddPdf model1("model1","model1",
//                      RooArgList(signal1,background),
//                      RooArgList(nsig1,nbkg1));
// 
//     RooRealVar nsig2("nsig2","signal yield",1000,0,1e6);
//     RooRealVar nbkg2("nbkg2","background yield",1000,0,1e6);
// 
//     RooAddPdf model2("model2","model2",
//                      RooArgList(signal1,background),
//                      RooArgList(nsig2,nbkg2));
//                      
//     model1.fitTo(data1);
//     model2.fitTo(data2);
// 
//     RooPlot *frame = x.frame();
// 
//     data1.plotOn(frame,MarkerColor(kBlack));
//     model1.plotOn(frame,LineColor(kBlue));
// 
//     data2.plotOn(frame,MarkerColor(kBlack));
//     model2.plotOn(frame,LineColor(kRed));
// 
//     model1.plotOn(frame,
//                   Components(background),
//                   LineStyle(kDashed),
//                   LineColor(kGreen+2)
//                   );
// 
//    	model2.plotOn(frame,
//    				  Components(background),
//    				  LineStyle(kDashed),
//    				  LineColor(kYellow+2)
//    				  );
// 
//     frame->SetTitle("X(3872)");
//     frame->Draw();

    auto legend = new TLegend(0.65,0.7,0.88,0.88);
    legend->AddEntry(h1,"Before BDT","signal");
    legend->AddEntry(h2,"After BDT","signal");
    legend->Draw();

//     TLatex latex ;
//     latex.SetNDC() ;
//     latex.SetTextFont(42) ;
// 
//     double step = 0.050 ; 
//     double x_res = 0.19 ; 
//     double y_res = 0.93 ; 
// 
//     /**
//     Info in <ProcessLinesForObject>: Obj  Execute ((TLatex *) 0x5cf08a011ee0)->SetX(0.15885416666666666);
//     Info in <ProcessLinesForObject>: Obj  Execute ((TLatex *) 0x5cf08a011ee0)->SetY(0.8536082474226804)
//     */
// 
//     latex.SetTextSize(0.040) ;
// 
//     latex.DrawLatex(x_res, y_res - step, Form("#mu_{Before BDT} = %.2f #pm %.2f", mean2.getVal(), mean2.getError()));
//     latex.DrawLatex(x_res, y_res - 2*step, Form("#sigma_{Before BDT} = %.2f #pm %.2f", sigma2.getVal(), sigma2.getError()));
//     latex.DrawLatex(x_res, y_res - 3*step, Form("N_{Before BDT} = %.0f #pm %.0f", nsig2.getVal(), nsig2.getError()));
// 
//     latex.DrawLatex(x_res, y_res - 5*step, Form("#mu_{After BDT} = %.2f #pm %.2f", mean1.getVal(), mean1.getError()));
//     latex.DrawLatex(x_res, y_res - 6*step, Form("#sigma_{After BDT} = %.2f #pm %.2f", sigma1.getVal(), sigma1.getError()));
//     latex.DrawLatex(x_res, y_res - 7*step, Form("N_{After BDT} = %.0f #pm %.0f", nsig1.getVal(), nsig1.getError()));
}
