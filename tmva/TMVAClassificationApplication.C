/// \file
/// \ingroup tutorial_tmva
/// \notebook -nodraw
/// This macro provides a simple example on how to use the trained classifiers
/// within an analysis module
/// - Project   : TMVA - a Root-integrated toolkit for multivariate data analysis
/// - Package   : TMVA
/// - Exectuable: TMVAClassificationApplication
///
/// \macro_output
/// \macro_code
/// \author Andreas Hoecker

#include <cstdlib>
#include <vector>
#include <iostream>
#include <map>
#include <string>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TStopwatch.h"

#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"

using namespace TMVA;

void TMVAClassificationApplication( TString myMethodList = "" )
{

   //---------------------------------------------------------------
   // This loads the library
   TMVA::Tools::Instance();

   // Default MVA methods to be trained + tested
   std::map<std::string,int> Use;

   // Cut optimisation
   Use["Cuts"]            = 1;
   Use["CutsD"]           = 1;
   Use["CutsPCA"]         = 0;
   Use["CutsGA"]          = 0;
   Use["CutsSA"]          = 0;
   //
   // 1-dimensional likelihood ("naive Bayes estimator")
   Use["Likelihood"]      = 1;
   Use["LikelihoodD"]     = 0; // the "D" extension indicates decorrelated input variables (see option strings)
   Use["LikelihoodPCA"]   = 1; // the "PCA" extension indicates PCA-transformed input variables (see option strings)
   Use["LikelihoodKDE"]   = 0;
   Use["LikelihoodMIX"]   = 0;
   //
   // Mutidimensional likelihood and Nearest-Neighbour methods
   Use["PDERS"]           = 1;
   Use["PDERSD"]          = 0;
   Use["PDERSPCA"]        = 0;
   Use["PDEFoam"]         = 1;
   Use["PDEFoamBoost"]    = 0; // uses generalised MVA method boosting
   Use["KNN"]             = 1; // k-nearest neighbour method
   //
   // Linear Discriminant Analysis
   Use["LD"]              = 1; // Linear Discriminant identical to Fisher
   Use["Fisher"]          = 0;
   Use["FisherG"]         = 0;
   Use["BoostedFisher"]   = 0; // uses generalised MVA method boosting
   Use["HMatrix"]         = 0;
   //
   // Function Discriminant analysis
   Use["FDA_GA"]          = 1; // minimisation of user-defined function using Genetics Algorithm
   Use["FDA_SA"]          = 0;
   Use["FDA_MC"]          = 0;
   Use["FDA_MT"]          = 0;
   Use["FDA_GAMT"]        = 0;
   Use["FDA_MCMT"]        = 0;
   //
   // Neural Networks (all are feed-forward Multilayer Perceptrons)
   Use["MLP"]             = 0; // Recommended ANN
   Use["MLPBFGS"]         = 0; // Recommended ANN with optional training method
   Use["MLPBNN"]          = 1; // Recommended ANN with BFGS training method and bayesian regulator
   Use["CFMlpANN"]        = 0; // Depreciated ANN from ALEPH
   Use["TMlpANN"]         = 0; // ROOT's own ANN
   Use["DNN_CPU"] = 0;         // CUDA-accelerated DNN training.
   Use["DNN_GPU"] = 0;         // Multi-core accelerated DNN.
   //
   // Support Vector Machine
   Use["SVM"]             = 1;
   //
   // Boosted Decision Trees
   Use["BDT"]             = 1; // uses Adaptive Boost
   Use["BDTG"]            = 0; // uses Gradient Boost
   Use["BDTB"]            = 0; // uses Bagging
   Use["BDTD"]            = 0; // decorrelation + Adaptive Boost
   Use["BDTF"]            = 0; // allow usage of fisher discriminant for node splitting
   //
   // Friedman's RuleFit method, ie, an optimised series of cuts ("rules")
   Use["RuleFit"]         = 1;
   // ---------------------------------------------------------------
   Use["Plugin"]          = 0;
   Use["Category"]        = 0;
   Use["SVM_Gauss"]       = 0;
   Use["SVM_Poly"]        = 0;
   Use["SVM_Lin"]         = 0;

   std::cout << std::endl;
   std::cout << "==> Start TMVAClassificationApplication" << std::endl;

   // Select methods (don't look at this code - not of interest)
   if (myMethodList != "") {
      for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) it->second = 0;

      std::vector<TString> mlist = gTools().SplitString( myMethodList, ',' );
      for (UInt_t i=0; i<mlist.size(); i++) {
         std::string regMethod(mlist[i]);

         if (Use.find(regMethod) == Use.end()) {
            std::cout << "Method \"" << regMethod
                      << "\" not known in TMVA under this name. Choose among the following:" << std::endl;
            for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
               std::cout << it->first << " ";
            }
            std::cout << std::endl;
            return;
         }
         Use[regMethod] = 1;
      }
   }

   // --------------------------------------------------------------------------------------------------

   // Create the Reader object

   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );

   // Create a set of variables and declare them to the reader
   // - the variable names MUST corresponds in name and type to those given in the weight file(s) used
   Float_t B_Jpsi_Chi2NDoF;
   Float_t B_ENDVERTEX_CHI2;
   Float_t sum_hadron_pT;
   Float_t B_FDchi2;
   Float_t B_DIRA;
   Float_t B_IPChi2;
   Float_t Mu1_IPCHI2_OWNPV;
   Float_t Mu2_IPCHI2_OWNPV;
   Float_t Pi1_IPCHI2_OWNPV;
   Float_t Pi2_IPCHI2_OWNPV;
   Float_t K_IPCHI2_OWNPV;
   Float_t Pi1_PT;
   Float_t Pi2_PT;
   Float_t Mu1_PT;
   Float_t Mu2_PT;
   Float_t K_PT;
   Float_t Min_Mu1_IPchi2_Mu2_IPchi2;
   Float_t Max_Mu1_IPchi2_Mu2_IPchi2;
   Float_t Min_Pi1_IPchi2_Pi2_IPchi2;
   Float_t Max_Pi1_IPchi2_Pi2_IPchi2;
   Float_t K_IPChi2;

   Double_t user_B_Jpsi_Chi2NDoF;
   Double_t user_B_ENDVERTEX_CHI2;
   Double_t user_sum_hadron_pT;
   Double_t user_B_FDchi2;
   Double_t user_B_DIRA;
   Double_t user_B_IPChi2;
   Double_t user_Mu1_IPCHI2_OWNPV;
   Double_t user_Mu2_IPCHI2_OWNPV;
   Double_t user_Pi1_IPCHI2_OWNPV;
   Double_t user_Pi2_IPCHI2_OWNPV;
   Double_t user_K_IPCHI2_OWNPV;
   Double_t user_Pi1_PT;
   Double_t user_Pi2_PT;
   Double_t user_Mu1_PT;
   Double_t user_Mu2_PT;
   Double_t user_K_PT;
   Double_t user_Min_Mu1_IPchi2_Mu2_IPchi2;
   Double_t user_Max_Mu1_IPchi2_Mu2_IPchi2;
   Double_t user_Min_Pi1_IPchi2_Pi2_IPchi2;
   Double_t user_Max_Pi1_IPchi2_Pi2_IPchi2;
   Double_t user_K_IPChi2;


   /*
   reader->AddVariable( "myvar1 := var1+var2", &var1 );
   reader->AddVariable( "myvar2 := var1-var2", &var2 );
   reader->AddVariable( "var3",                &var3 );
   reader->AddVariable( "var4",                &var4 );
   */

   reader->AddVariable("log(B_Jpsi_Chi2NDoF)",                        &B_Jpsi_Chi2NDoF);
   reader->AddVariable("log(B_ENDVERTEX_CHI2)",                       &B_ENDVERTEX_CHI2);
   reader->AddVariable("log(Pi1_PT+Pi2_PT+K_PT)",                     &sum_hadron_pT);
   reader->AddVariable("log(B_FDCHI2_OWNPV)",                         &B_FDchi2);
   reader->AddVariable("log(1-B_DIRA_OWNPV)",                         &B_DIRA);
   reader->AddVariable("log(B_IPCHI2_OWNPV)",                         &B_IPChi2);
   reader->AddVariable("log(min(Mu1_IPCHI2_OWNPV,Mu2_IPCHI2_OWNPV))", &Min_Mu1_IPchi2_Mu2_IPchi2);
   reader->AddVariable("log(max(Mu1_IPCHI2_OWNPV,Mu2_IPCHI2_OWNPV))", &Max_Mu1_IPchi2_Mu2_IPchi2);
   reader->AddVariable("log(min(Pi1_IPCHI2_OWNPV,Pi2_IPCHI2_OWNPV))", &Min_Pi1_IPchi2_Pi2_IPchi2);
   reader->AddVariable("log(max(Pi1_IPCHI2_OWNPV,Pi2_IPCHI2_OWNPV))", &Max_Pi1_IPchi2_Pi2_IPchi2);
   reader->AddVariable("log(K_IPCHI2_OWNPV)",                         &K_IPChi2);

   // Spectator variables declared in the training have to be added to the reader, too
   //Float_t spec1,spec2;
   //reader->AddSpectator( "spec1 := var1*2",   &spec1 );
   //reader->AddSpectator( "spec2 := var1*3",   &spec2 );

   //Float_t Category_cat1, Category_cat2, Category_cat3;
   //if (Use["Category"]){
      // Add artificial spectators for distinguishing categories
   //   reader->AddSpectator( "Category_cat1 := var3<=0",             &Category_cat1 );
   //   reader->AddSpectator( "Category_cat2 := (var3>0)&&(var4<0)",  &Category_cat2 );
   //   reader->AddSpectator( "Category_cat3 := (var3>0)&&(var4>=0)", &Category_cat3 );
   //}

   // Book the MVA methods

   TString dir    = "dataset/weights/";
   TString prefix = "TMVAClassification";

   // Book method(s)
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
         TString methodName = TString(it->first) + TString(" method");
         TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
         reader->BookMVA( methodName, weightfile );
      }
   }

   // Book output histograms
   UInt_t nbin = 100;
   // TH1F *histLk(0);
   // TH1F *histLkD(0);
   // TH1F *histLkPCA(0);
   // TH1F *histLkKDE(0);
   // TH1F *histLkMIX(0);
   // TH1F *histPD(0);
   // TH1F *histPDD(0);
   // TH1F *histPDPCA(0);
   // TH1F *histPDEFoam(0);
   // TH1F *histPDEFoamErr(0);
   // TH1F *histPDEFoamSig(0);
   // TH1F *histKNN(0);
   // TH1F *histHm(0);
   // TH1F *histFi(0);
   // TH1F *histFiG(0);
   // TH1F *histFiB(0);
   // TH1F *histLD(0);
   // TH1F *histNn(0);
   // TH1F *histNnbfgs(0);
   // TH1F *histNnbnn(0);
   // TH1F *histNnC(0);
   // TH1F *histNnT(0);
   // TH1F *histBdt(0);
   // TH1F *histBdtG(0);
   // TH1F *histBdtB(0);
   // TH1F *histBdtD(0);
   // TH1F *histBdtF(0);
   // TH1F *histRf(0);
   // TH1F *histSVMG(0);
   // TH1F *histSVMP(0);
   // TH1F *histSVML(0);
   // TH1F *histFDAMT(0);
   // TH1F *histFDAGA(0);
   // TH1F *histCat(0);
   // TH1F *histPBdt(0);
   // TH1F *histDnnGpu(0);
   // TH1F *histDnnCpu(0);

   // if (Use["Likelihood"])    histLk      = new TH1F( "MVA_Likelihood",    "MVA_Likelihood",    nbin, -1, 1 );
   // if (Use["LikelihoodD"])   histLkD     = new TH1F( "MVA_LikelihoodD",   "MVA_LikelihoodD",   nbin, -1, 0.9999 );
   // if (Use["LikelihoodPCA"]) histLkPCA   = new TH1F( "MVA_LikelihoodPCA", "MVA_LikelihoodPCA", nbin, -1, 1 );
   // if (Use["LikelihoodKDE"]) histLkKDE   = new TH1F( "MVA_LikelihoodKDE", "MVA_LikelihoodKDE", nbin,  -0.00001, 0.99999 );
   // if (Use["LikelihoodMIX"]) histLkMIX   = new TH1F( "MVA_LikelihoodMIX", "MVA_LikelihoodMIX", nbin,  0, 1 );
   // if (Use["PDERS"])         histPD      = new TH1F( "MVA_PDERS",         "MVA_PDERS",         nbin,  0, 1 );
   // if (Use["PDERSD"])        histPDD     = new TH1F( "MVA_PDERSD",        "MVA_PDERSD",        nbin,  0, 1 );
   // if (Use["PDERSPCA"])      histPDPCA   = new TH1F( "MVA_PDERSPCA",      "MVA_PDERSPCA",      nbin,  0, 1 );
   // if (Use["KNN"])           histKNN     = new TH1F( "MVA_KNN",           "MVA_KNN",           nbin,  0, 1 );
   // if (Use["HMatrix"])       histHm      = new TH1F( "MVA_HMatrix",       "MVA_HMatrix",       nbin, -0.95, 1.55 );
   // if (Use["Fisher"])        histFi      = new TH1F( "MVA_Fisher",        "MVA_Fisher",        nbin, -4, 4 );
   // if (Use["FisherG"])       histFiG     = new TH1F( "MVA_FisherG",       "MVA_FisherG",       nbin, -1, 1 );
   // if (Use["BoostedFisher"]) histFiB     = new TH1F( "MVA_BoostedFisher", "MVA_BoostedFisher", nbin, -2, 2 );
   // if (Use["LD"])            histLD      = new TH1F( "MVA_LD",            "MVA_LD",            nbin, -2, 2 );
   // if (Use["MLP"])           histNn      = new TH1F( "MVA_MLP",           "MVA_MLP",           nbin, -1.25, 1.5 );
   // if (Use["MLPBFGS"])       histNnbfgs  = new TH1F( "MVA_MLPBFGS",       "MVA_MLPBFGS",       nbin, -1.25, 1.5 );
   // if (Use["MLPBNN"])        histNnbnn   = new TH1F( "MVA_MLPBNN",        "MVA_MLPBNN",        nbin, -1.25, 1.5 );
   // if (Use["CFMlpANN"])      histNnC     = new TH1F( "MVA_CFMlpANN",      "MVA_CFMlpANN",      nbin,  0, 1 );
   // if (Use["TMlpANN"])       histNnT     = new TH1F( "MVA_TMlpANN",       "MVA_TMlpANN",       nbin, -1.3, 1.3 );
   // if (Use["DNN_GPU"]) histDnnGpu = new TH1F("MVA_DNN_GPU", "MVA_DNN_GPU", nbin, -0.1, 1.1);
   // if (Use["DNN_CPU"]) histDnnCpu = new TH1F("MVA_DNN_CPU", "MVA_DNN_CPU", nbin, -0.1, 1.1);
   // if (Use["BDT"])           histBdt     = new TH1F( "MVA_BDT",           "MVA_BDT",           nbin, -0.8, 0.8 );
   // if (Use["BDTG"])          histBdtG    = new TH1F( "MVA_BDTG",          "MVA_BDTG",          nbin, -1.0, 1.0 );
   // if (Use["BDTB"])          histBdtB    = new TH1F( "MVA_BDTB",          "MVA_BDTB",          nbin, -1.0, 1.0 );
   // if (Use["BDTD"])          histBdtD    = new TH1F( "MVA_BDTD",          "MVA_BDTD",          nbin, -0.8, 0.8 );
   // if (Use["BDTF"])          histBdtF    = new TH1F( "MVA_BDTF",          "MVA_BDTF",          nbin, -1.0, 1.0 );
   // if (Use["RuleFit"])       histRf      = new TH1F( "MVA_RuleFit",       "MVA_RuleFit",       nbin, -2.0, 2.0 );
   // if (Use["SVM_Gauss"])     histSVMG    = new TH1F( "MVA_SVM_Gauss",     "MVA_SVM_Gauss",     nbin,  0.0, 1.0 );
   // if (Use["SVM_Poly"])      histSVMP    = new TH1F( "MVA_SVM_Poly",      "MVA_SVM_Poly",      nbin,  0.0, 1.0 );
   // if (Use["SVM_Lin"])       histSVML    = new TH1F( "MVA_SVM_Lin",       "MVA_SVM_Lin",       nbin,  0.0, 1.0 );
   // if (Use["FDA_MT"])        histFDAMT   = new TH1F( "MVA_FDA_MT",        "MVA_FDA_MT",        nbin, -2.0, 3.0 );
   // if (Use["FDA_GA"])        histFDAGA   = new TH1F( "MVA_FDA_GA",        "MVA_FDA_GA",        nbin, -2.0, 3.0 );
   // if (Use["Category"])      histCat     = new TH1F( "MVA_Category",      "MVA_Category",      nbin, -2., 2. );
   // if (Use["Plugin"])        histPBdt    = new TH1F( "MVA_PBDT",          "MVA_BDT",           nbin, -0.8, 0.8 );

   // // PDEFoam also returns per-event error, fill in histogram, and also fill significance
   // if (Use["PDEFoam"]) {
   //    histPDEFoam    = new TH1F( "MVA_PDEFoam",       "MVA_PDEFoam",              nbin,  0, 1 );
   //    histPDEFoamErr = new TH1F( "MVA_PDEFoamErr",    "MVA_PDEFoam error",        nbin,  0, 1 );
   //    histPDEFoamSig = new TH1F( "MVA_PDEFoamSig",    "MVA_PDEFoam significance", nbin,  0, 10 );
   // }

   // // Book example histogram for probability (the other methods are done similarly)
   // TH1F *probHistFi(0), *rarityHistFi(0);
   // if (Use["Fisher"]) {
   //    probHistFi   = new TH1F( "MVA_Fisher_Proba",  "MVA_Fisher_Proba",  nbin, 0, 1 );
   //    rarityHistFi = new TH1F( "MVA_Fisher_Rarity", "MVA_Fisher_Rarity", nbin, 0, 1 );
   // }

   // Prepare input tree (this must be replaced by your data source)
   // in this example, there is a toy tree with signal and one with background events
   // we'll later on use only the "signal" events for the test in this example.
   //
   TFile *input(0);
   TString fname = "./Bu2JpsiKX_all_pp.root";
   if (!gSystem->AccessPathName( fname )) {
      input = TFile::Open( fname ); // check if file in local directory exists
   }
   else {
      TFile::SetCacheFileDir(".");
      input = TFile::Open("./Bu2JpsiKX_all_pp.root", "CACHEREAD"); // if not: download from ROOT server
   }
   if (!input) {
      std::cout << "ERROR: could not open data file" << std::endl;
      exit(1);
   }
   std::cout << "--- TMVAClassificationApp    : Using input file: " << input->GetName() << std::endl;

   // Event loop

   // Prepare the event tree
   // - Here the variable names have to corresponds to your tree
   // - You can use the same variables as above which is slightly faster,
   //   but of course you can use different ones and copy the values inside the event loop
   //
   std::cout << "--- Select signal sample" << std::endl;
   TTree* theTree = (TTree*)input->Get("MyTuple");

   theTree->SetBranchAddress("B_Jpsi_Chi2NDoF", &user_B_Jpsi_Chi2NDoF  );//,kFloat_t);//,"B_Jpsi_Chi2NDoF/F");
   theTree->SetBranchAddress("B_ENDVERTEX_CHI2",&user_B_ENDVERTEX_CHI2 );//,kFloat_t);//"B_ENDVERTEX_CHI2/F");
   theTree->SetBranchAddress("Pi1_PT",          &user_Pi1_PT           );//,"Pi1_PT/F");
   theTree->SetBranchAddress("Pi2_PT",          &user_Pi2_PT           );//,"Pi2_PT/F");
   theTree->SetBranchAddress("K_PT",            &user_K_PT             );//,"K_PT/F");
   theTree->SetBranchAddress("B_FDCHI2_OWNPV",  &user_B_FDchi2         );//,"B_FDCHI2_OWNPV/F");
   theTree->SetBranchAddress("B_DIRA_OWNPV",    &user_B_DIRA           );//,"B_DIRA/F");
   theTree->SetBranchAddress("B_IPCHI2_OWNPV",  &user_B_IPChi2         );//,"B_IPCHI2_OWNPV/F");
   theTree->SetBranchAddress("Mu1_IPCHI2_OWNPV",&user_Mu1_IPCHI2_OWNPV );//,"Mu1_IPCHI2_OWNPV/F");
   theTree->SetBranchAddress("Mu2_IPCHI2_OWNPV",&user_Mu2_IPCHI2_OWNPV );//,"Mu2_IPCHI2_OWNPV/F");
   theTree->SetBranchAddress("Pi1_IPCHI2_OWNPV",&user_Pi1_IPCHI2_OWNPV );//,"Pi1_IPCHI2_OWNPV/F");
   theTree->SetBranchAddress("Pi2_IPCHI2_OWNPV",&user_Pi2_IPCHI2_OWNPV );//,"Pi2_IPCHI2_OWNPV/F");
   theTree->SetBranchAddress("K_IPCHI2_OWNPV",  &user_K_IPCHI2_OWNPV   );//,"K_IPCHI2_OWNPV/F");

   // Efficiency calculator for cut method
   Int_t    nSelCutsGA = 0;
   Double_t effS       = 0.7;

   Double_t myBDT = 0.0;

   TFile *fileOut = new TFile("BDT_data_output.root","RECREATE");
   TTree *treeOut = new TTree("MyTuple","BDT output");

   treeOut->Branch("BDT",&myBDT,"BDT/D");
   std::vector<Float_t> vecVar(4); // vector for EvaluateMVA tests

   std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
   TStopwatch sw;
   sw.Start();
   for (Long64_t ievt=0; ievt<theTree->GetEntries();ievt++) {

      if (ievt%10000 == 0) std::cout << "--- ... Processing event: " << ievt << std::endl;
      theTree->GetEntry(ievt);
      if(user_B_Jpsi_Chi2NDoF < 0.0)
	{
	  user_B_Jpsi_Chi2NDoF = 100000;
	}
      B_Jpsi_Chi2NDoF  = float(log(user_B_Jpsi_Chi2NDoF));
      B_ENDVERTEX_CHI2 = float(log(user_B_ENDVERTEX_CHI2));
      sum_hadron_pT    = float(log(user_Pi1_PT+user_Pi2_PT+user_K_PT));

      // Pi1_PT           = float(user_Pi1_PT);
      // Pi2_PT           = float(user_Pi2_PT);
      // K_PT             = float(user_K_PT);

      B_FDchi2         = float(log(user_B_FDchi2));
      B_DIRA           = float(log(1-user_B_DIRA));
      B_IPChi2         = float(log(user_B_IPChi2));

      Min_Mu1_IPchi2_Mu2_IPchi2 = float(log(min(user_Mu1_IPCHI2_OWNPV,user_Mu2_IPCHI2_OWNPV)));
      // Mu1_IPCHI2_OWNPV = float(user_Mu1_IPCHI2_OWNPV);
      // Mu2_IPCHI2_OWNPV = float(user_Mu2_IPCHI2_OWNPV);
      Max_Mu1_IPchi2_Mu2_IPchi2 = float(log(max(user_Mu1_IPCHI2_OWNPV,user_Mu2_IPCHI2_OWNPV)));

      Min_Pi1_IPchi2_Pi2_IPchi2 = float(log(min(user_Pi1_IPCHI2_OWNPV,user_Pi2_IPCHI2_OWNPV)));
      // Pi1_IPCHI2_OWNPV = float(user_Pi1_IPCHI2_OWNPV);
      // Pi2_IPCHI2_OWNPV = float(user_Pi2_IPCHI2_OWNPV);
      Max_Pi1_IPchi2_Pi2_IPchi2 = float(log(max(user_Pi1_IPCHI2_OWNPV,user_Pi2_IPCHI2_OWNPV)));
      K_IPCHI2_OWNPV   = float(log(user_K_IPCHI2_OWNPV));

      //var1 = userVar1 + userVar2;
      //var2 = userVar1 - userVar2;

      // Return the MVA outputs and fill into histograms

      myBDT = reader->EvaluateMVA("BDT method");
      treeOut->Fill();
      //      if (Use["BDT"])histBdt->Fill(reader->EvaluateMVA("BDT method") );
      
   }
   
   // Get elapsed time
   sw.Stop();
   std::cout << "--- End of event loop: "; sw.Print();

   // Get efficiency for cuts classifier
   
   // Write histograms

   // TFile *target  = new TFile( "TMVApp.root","RECREATE" );
   
   // if (Use["BDT"          ])   histBdt    ->Write();
   
   // target->Close();

   fileOut->cd();
   treeOut->Write();
   fileOut->Close();

   std::cout << "--- Created root file: \"TMVApp.root\" containing the MVA output histograms" << std::endl;

   delete reader;

   std::cout << "==> TMVAClassificationApplication is done!" << std::endl << std::endl;
}

int main( int argc, char** argv )
{
   TString methodList;
   for (int i=1; i<argc; i++) {
      TString regMethod(argv[i]);
      if(regMethod=="-b" || regMethod=="--batch") continue;
      if (!methodList.IsNull()) methodList += TString(",");
      methodList += regMethod;
   }
   TMVAClassificationApplication(methodList);
   return 0;
}
