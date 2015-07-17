#include <sstream>

int sample9(){

  ESRLine esr( "cal2_1.txt" );

  double l1 = esr.find( 319.7, 319.85 );
  double l2 = esr.find( 321.2, 321.4 );
  double l3 = esr.find( 322.75, 322.95 );

  TGraph *g = (TGraph*) esr.GetGraph()->Clone();
  
  g->Draw( "Al" );

  double xmax = g->GetXaxis()->GetXmax();
  double xmin = g->GetXaxis()->GetXmin();
  double dx = 0.1 * ( xmax - xmin );

  double ymax = g->GetYaxis()->GetXmax();
  double ymin = g->GetYaxis()->GetXmin();
  double dy = 0.1 * ( ymax - ymin );

  TLine *line = new TLine;

  line->SetLineWidth( 1 );
  line->SetLineStyle( 2 );
  line->SetLineColor( kGray + 3 );
  line->DrawLine( xmax, 0.0, xmin, 0.0 );
  
  line->SetLineWidth( 1 );
  line->SetLineStyle( 1 );
  line->SetLineColor( kGreen + 3 );
  line->DrawLine( l1, ymin, l1, ymax );
  line->DrawLine( l2, ymin, l2, ymax );
  line->DrawLine( l3, ymin, l3, ymax );

  TLatex *latex = new TLatex;
  
  ostringstream  ostr;
  ostr << "1: " << l1 << " mT";
  latex->DrawLatex( xmin + 7.0 * dx, ymin + 8.5 * dy, ostr.str().c_str() );

  ostr.str( "" );
  ostr << "1: " << l2 << " mT";
  latex->DrawLatex( xmin + 7.0 * dx, ymin + 7.5 * dy, ostr.str().c_str() );

  ostr.str( "" );
  ostr << "1: " << l3 << " mT";
  latex->DrawLatex( xmin + 7.0 * dx, ymin + 6.5 * dy, ostr.str().c_str() );

  
  return 0;
}
