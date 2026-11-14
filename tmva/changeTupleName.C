void changeTupleName()
{
    TFile *f = new TFile("Bu2JpsiKX_all_pp.root", "UPDATE");

    TTree *t;
    f->GetObject("X_Mass_DTF", t); 

    if (t) {
        t->SetName("X_M");

        t->Write("", TObject::kOverwrite); 
    }

    f->Close();
}
