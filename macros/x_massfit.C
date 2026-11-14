#include "lhcbStyle.C"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <cmath>

void x_massfit()
{
    using namespace RooFit;
    lhcbStyle();

    const double x_min = 3800;
    const double x_max = 3950;

    // Read CSV file
    std::ifstream csv("../data/collision_data.csv");

    if (!csv.is_open()) {
        std::cerr << "Error: Cannot open collision_data.csv" << std::endl;
        return;
    }

    // Define observables
    RooRealVar x_mass(
        "X_Mass_DTF",
        "M(J/#psi#pi^{+}#pi^{-}) [MeV/c^{2}]",
        x_min, x_max
    );

    RooRealVar b_mass(
        "B_Jpsi_B_M",
        "M(B) [MeV/c^{2}]",
        5200, 5350
    );

    // Create empty RooDataSet
    RooDataSet raw_data(
        "raw_data",
        "Data from collision_data.csv",
        RooArgSet(x_mass, b_mass)
    );

    std::string line;

    // Skip CSV header
    std::getline(csv, line);

    int total_entries = 0;
    int accepted_entries = 0;

    // Read CSV rows
    while (std::getline(csv, line)) {

        if (line.empty()) continue;

        std::stringstream ss(line);

        std::string b_mass_str;
        std::string x_mass_str;

        if (!std::getline(ss, b_mass_str, ',')) continue;
        if (!std::getline(ss, x_mass_str, ',')) continue;

        try {
            double b_value = std::stod(b_mass_str);
            double x_value = std::stod(x_mass_str);

            total_entries++;

            // Check observable ranges
            if (!std::isfinite(b_value) || !std::isfinite(x_value))
                continue;

            if (b_value < 5200 || b_value > 5350)
                continue;

            if (x_value < x_min || x_value > x_max)
                continue;

            b_mass.setVal(b_value);
            x_mass.setVal(x_value);

            raw_data.add(RooArgSet(x_mass, b_mass));

            accepted_entries++;
        }
        catch (...) {
            std::cerr << "Warning: Invalid CSV row skipped: "
                      << line << std::endl;
        }
    }

    csv.close();

    std::cout << "Total CSV entries: " << total_entries << std::endl;
    std::cout << "Accepted entries: " << accepted_entries << std::endl;

    if (accepted_entries == 0) {
        std::cerr << "Error: No valid entries found!" << std::endl;
        return;
    }

    // B mass selection
    RooDataSet *data = (RooDataSet *)raw_data.reduce(
        Cut("abs(B_Jpsi_B_M - 5279.84) < (2.0 * 7.16)")
    );

    std::cout << "Entries after B mass selection: "
              << data->numEntries() << std::endl;

    if (data->numEntries() == 0) {
        std::cerr << "Error: No entries after B mass selection!" << std::endl;
        return;
    }

    // Signal: Double-Sided Crystal Ball
    RooRealVar mean("mean", "mean", 3872, 3865, 3880);

    RooRealVar sigma_L("sigma_L", "sigma_L", 3.0, 0.5, 10.0);
    RooRealVar sigma_R("sigma_R", "sigma_R", 3.0, 0.5, 10.0);

    RooRealVar aL("aL", "alphaL", 1.2, 0.1, 5.0);
    RooRealVar nL("nL", "nL", 2.0, 0.1, 50.0);

    RooRealVar aR("aR", "alphaR", 1.2, 0.1, 5.0);
    RooRealVar nR("nR", "nR", 2.0, 0.1, 50.0);

    RooCrystalBall signal(
        "signal",
        "X(3872) DSCB",
        x_mass,
        mean,
        sigma_L,
        sigma_R,
        aL,
        nL,
        aR,
        nR
    );

    // Background: Exponential
    RooRealVar tau("tau", "tau", -0.001, -0.1, 0.1);

    RooExponential bkg(
        "bkg",
        "Background",
        x_mass,
        tau
    );

    // Extended model
    RooRealVar nsig("nsig", "Yield X(3872)", 5000, 0, 100000);
    RooRealVar nbkg("nbkg", "Yield Background", 20000, 0, 500000);

    RooAddPdf model(
        "model",
        "Full Model",
        RooArgList(signal, bkg),
        RooArgList(nsig, nbkg)
    );

    // Fit
    RooFitResult *fitResult = model.fitTo(
        *data,
        Extended(kTRUE),
        Save()
    );

    fitResult->Print();

    // Canvas
    TCanvas *c = new TCanvas("c", "X(3872) Fit", 800, 600);

    TPad *pad1 = new TPad("pad1", "pad1", 0.0, 0.3, 1.0, 1.0);
    TPad *pad2 = new TPad("pad2", "pad2", 0.0, 0.05, 1.0, 0.3);

    pad1->SetBottomMargin(0.02);
    pad2->SetTopMargin(0.05);
    pad2->SetBottomMargin(0.3);

    pad1->Draw();
    pad2->Draw();

    // Main mass distribution
    pad1->cd();

    RooPlot *frame = x_mass.frame(Bins(75));

    data->plotOn(frame, Name("data"));

    model.plotOn(
        frame,
        Name("model"),
        LineColor(kBlue)
    );

    model.plotOn(
        frame,
        Name("sig"),
        Components(signal),
        LineColor(kRed)
    );

    model.plotOn(
        frame,
        Name("bkg_comp"),
        Components(bkg),
        LineStyle(kDashed),
        LineColor(kGreen + 2)
    );

    frame->GetXaxis()->SetLabelSize(0);
    frame->GetYaxis()->SetTitle("Candidates / 2.0 [MeV/c^{2}]");

    frame->Draw();

    // Legend
    TLegend *leg = new TLegend(0.65, 0.55, 0.88, 0.88);

    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.045);

    leg->AddEntry("data", "Data", "lep");
    leg->AddEntry("model", "Total Fit", "l");
    leg->AddEntry("sig", "X(3872) Signal", "l");
    leg->AddEntry("bkg_comp", "Background", "l");

    leg->Draw();

    // LHCb label
    TLatex latex;

    latex.SetNDC();
    latex.SetTextFont(42);
    latex.SetTextSize(0.065);

    latex.DrawLatex(0.18, 0.85, "#bf{LHCb} 5.6 fb^{-1}");

    // Fit parameters
    double x_pos = 0.66;
    double y_pos = 0.50;
    double dy = 0.045;

    latex.SetTextSize(0.055);

    latex.DrawLatex(
        x_pos, y_pos,
        Form("#mu = %.2f #pm %.2f",
             mean.getVal(), mean.getError())
    );

    latex.DrawLatex(
        x_pos, y_pos - dy,
        Form("#sigma_{L} = %.2f #pm %.2f",
             sigma_L.getVal(), sigma_L.getError())
    );

    latex.DrawLatex(
        x_pos, y_pos - 2 * dy,
        Form("#sigma_{R} = %.2f #pm %.2f",
             sigma_R.getVal(), sigma_R.getError())
    );

    latex.DrawLatex(
        x_pos, y_pos - 3 * dy,
        Form("N_{sig} = %.0f #pm %.0f",
             nsig.getVal(), nsig.getError())
    );

    // Pull distribution
    pad2->cd();

    RooHist *hpull = frame->pullHist("data", "model");

    RooPlot *pullFrame = x_mass.frame(Title(" "));

    pullFrame->addPlotable(hpull, "P");

    pullFrame->SetMinimum(-5);
    pullFrame->SetMaximum(5);

    pullFrame->GetYaxis()->SetNdivisions(505);
    pullFrame->GetYaxis()->SetTitleSize(0.12);
    pullFrame->GetYaxis()->SetLabelSize(0.1);

    pullFrame->GetXaxis()->SetTitleSize(0.12);
    pullFrame->GetXaxis()->SetLabelSize(0.1);

    pullFrame->Draw();

    // Pull reference lines
    TLine *l0 = new TLine(x_min, 0, x_max, 0);
    TLine *l1 = new TLine(x_min, 3, x_max, 3);
    TLine *l2 = new TLine(x_min, -3, x_max, -3);

    l1->SetLineStyle(kDashed);
    l2->SetLineStyle(kDashed);

    l1->SetLineColor(kRed);
    l2->SetLineColor(kRed);

    l0->Draw("same");
    l1->Draw("same");
    l2->Draw("same");

    c->Update();
}
