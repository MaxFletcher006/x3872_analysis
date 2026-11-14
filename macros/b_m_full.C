#include "lhcbStyle.C"

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

void b_m_full() {

    using namespace RooFit;
    lhcbStyle();

    const double min = 5200;
    const double max = 5350;

    // Read CSV file
    std::ifstream file("../data/collision_data.csv");

    if (!file.is_open()) {
        std::cout << "Cannot open collision_data.csv" << std::endl;
        return;
    }

    // Define RooFit variable
    RooRealVar mass(
        "B_Jpsi_B_M",
        "M(#pi^{+}#pi^{-}#mu^{+}#mu^{-}K^{+}) [MeV/c^{2}]",
        min,
        max
    );

    // Create RooDataSet
    RooDataSet data(
        "data",
        "Collision data",
        RooArgSet(mass)
    );

    // Skip CSV header
    std::string line;
    std::getline(file, line);

    int totalEntries = 0;
    int acceptedEntries = 0;

    // Read CSV rows
    while (std::getline(file, line)) {

        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string b_mass_str;
        std::string x_mass_str;

        if (!std::getline(ss, b_mass_str, ',')) continue;
        if (!std::getline(ss, x_mass_str, ',')) continue;

        try {

            double b_mass = std::stod(b_mass_str);

            totalEntries++;

            if (b_mass < min || b_mass > max) continue;

            mass.setVal(b_mass);
            data.add(RooArgSet(mass));

            acceptedEntries++;

        }
        catch (const std::exception& e) {
            std::cout << "Invalid CSV row: " << line << std::endl;
        }
    }

    file.close();

    std::cout << "Total entries: " << totalEntries << std::endl;
    std::cout << "Accepted entries: " << acceptedEntries << std::endl;

    if (data.numEntries() == 0) {
        std::cout << "No events found in the selected mass range!" << std::endl;
        return;
    }

    // Signal parameters
    RooRealVar mean("mean", "Mean", 5279, min, max);
    RooRealVar sigmaL("sigmaL", "sigmaL", 8.0, 1.0, 30.0);
    RooRealVar sigmaR("sigmaR", "sigmaR", 8.0, 1.0, 30.0);

    RooRealVar alphaL("alphaL", "alphaL", 1.5, 0.5, 5.0);
    RooRealVar nL("nL", "nL", 3.0, 0.5, 20.0);

    RooRealVar alphaR("alphaR", "alphaR", 2.0, 0.5, 5.0);
    RooRealVar nR("nR", "nR", 3.0, 0.5, 20.0);

    RooCrystalBall signal(
        "Signal",
        "Signal PDF",
        mass,
        mean,
        sigmaL,
        sigmaR,
        alphaL,
        nL,
        alphaR,
        nR
    );

    // Background
    RooRealVar tau("tau", "Background slope", -0.002, -0.01, 0.0);

    RooExponential background(
        "background",
        "Background PDF",
        mass,
        tau
    );

    // Signal and background yields
    RooRealVar nsig(
        "nsig",
        "Signal yield",
        data.numEntries() * 0.6,
        0,
        data.numEntries() * 1.5
    );

    RooRealVar nbkg(
        "nbkg",
        "Background yield",
        data.numEntries() * 0.4,
        0,
        data.numEntries() * 1.5
    );

    RooAddPdf model(
        "model",
        "Signal + Background",
        RooArgList(signal, background),
        RooArgList(nsig, nbkg)
    );

    // Fit
    model.fitTo(data, Extended(true));

    // Canvas
    TCanvas *c = new TCanvas("c", "B mass fit", 800, 600);

    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.30, 1, 1);
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.00, 1, 0.30);

    pad1->SetBottomMargin(0.02);
    pad2->SetTopMargin(0.05);
    pad2->SetBottomMargin(0.30);

    pad1->Draw();
    pad2->Draw();

    // Main plot
    pad1->cd();

    RooPlot *frame1 = mass.frame();

    data.plotOn(
        frame1,
        Binning(50),
        DataError(RooAbsData::Poisson),
        Name("data")
    );

    model.plotOn(
        frame1,
        LineWidth(2),
        Name("model")
    );

    model.plotOn(
        frame1,
        Components(background),
        LineStyle(kDashed),
        LineColor(kGreen + 2),
        Name("bkg")
    );

    model.plotOn(
        frame1,
        Components(signal),
        LineColor(kRed),
        Name("sig")
    );

    frame1->GetXaxis()->SetLabelSize(0);
    frame1->GetYaxis()->SetTitle("Candidates / 3.0 [MeV/c^{2}]");

    frame1->Draw();

    // Legend
    TLegend *leg = new TLegend(0.65, 0.55, 0.88, 0.88);

    leg->SetBorderSize(0);
    leg->SetFillStyle(0);
    leg->SetTextFont(42);
    leg->SetTextSize(0.045);

    leg->AddEntry("data", "Data", "lep");
    leg->AddEntry("model", "Total fit", "l");
    leg->AddEntry("sig", "Signal", "l");
    leg->AddEntry("bkg", "Background", "l");

    leg->Draw();

    // Fit results
    TLatex latex;

    latex.SetNDC();
    latex.SetTextFont(42);
    latex.SetTextSize(0.065);

    latex.DrawLatex(0.38, 0.68, "#bf{LHCb} 5.6 fb^{-1}");

    double x_pos = 0.38;
    double y_start = 0.75;
    double y_step = 0.045;

    latex.SetTextSize(0.055);

    latex.DrawLatex(
        x_pos,
        y_start,
        Form("#mu = %.2f #pm %.2f", mean.getVal(), mean.getError())
    );

    latex.DrawLatex(
        x_pos,
        y_start - 2*y_step,
        Form("#sigma = %.2f", sigmaL.getVal() + sigmaR.getVal())
    );

    latex.DrawLatex(
        x_pos,
        y_start - 3*y_step,
        Form("N_{sig} = %.0f #pm %.0f", nsig.getVal(), nsig.getError())
    );

    // Pull plot
    pad2->cd();

    RooHist *hpull = frame1->pullHist("data", "model");

    RooPlot *frame2 = mass.frame();

    frame2->addPlotable(hpull, "P");

    frame2->GetYaxis()->SetTitle("Pull");
    frame2->GetYaxis()->SetNdivisions(505);

    frame2->GetYaxis()->SetTitleSize(0.12);
    frame2->GetYaxis()->SetLabelSize(0.10);

    frame2->GetXaxis()->SetTitleSize(0.12);
    frame2->GetXaxis()->SetLabelSize(0.10);

    frame2->SetMinimum(-5);
    frame2->SetMaximum(5);

    frame2->Draw();

    // Reference lines
    TLine *l0 = new TLine(min, 0, max, 0);
    TLine *l1 = new TLine(min, 3, max, 3);
    TLine *l2 = new TLine(min, -3, max, -3);

    l1->SetLineStyle(kDashed);
    l2->SetLineStyle(kDashed);

    l1->SetLineColor(kRed);
    l2->SetLineColor(kRed);

    l0->Draw("same");
    l1->Draw("same");
    l2->Draw("same");

    c->Update();
}
