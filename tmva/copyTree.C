void copyTree(){

	TFile oldfile("./Bu2JpsiKX_all_pp.root","read");
	TTree *oldtree;
	oldfile.GetObject("MyTuple",oldtree);

	oldtree->SetBranchStatus("*",0);

	for (auto activeBranchName: {
			"B_Jpsi_B_M",
			"X_Mass_DTF",
			"B_Jpsi_Chi2NDoF",
			"B_ENDVERTEX_CHI2",
			"Pi1_PT",
			"Pi2_PT",
			"K_PT",
			"B_FDCHI2_OWNPV",
			"B_DIRA_OWNPV",
			"B_IPCHI2_OWNPV",
			"Mu1_IPCHI2_OWNPV",
			"Mu2_IPCHI2_OWNPV",
			"Pi1_IPCHI2_OWNPV",
			"Pi2_IPCHI2_OWNPV",
			"K_IPCHI2_OWNPV",
			"Jpsi_M",
	})	

	oldtree->SetBranchStatus(activeBranchName,1);

	TFile newfile("Bu2JpsiK_reduced.root","RECREATE");
	auto newtree = oldtree->CloneTree();

	newtree->Print();
	newfile.Write();
}
