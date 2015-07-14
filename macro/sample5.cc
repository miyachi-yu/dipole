/* ----------------------------------------------------------------
   file:         sample5.cc
   description: 
   Example for ESR data handling. Details of the ESR class can be
   found in Quark Group Wiki Page
   http://www.quark.kj.yamagata-u.ac.jp/dokuwiki/doku.php?id=quark:pt:esr

   esr2.zip was imported to this analsys software.
   ---------------------------------------------------------------- */
int sample5(){

  MyApplication *app = MyApplication::instance();

  // ESR spectrum of TEMPO doped material
  ESR esr( "cal2_1.txt", 64 ); // prepare ESR data from the given file

  // ESR lines are at 319.8, 321.3 and 322.8
  app->toffset( 319.8 );
  app->toffset( 321.3 );
  app->toffset( 322.8 );
  
  app->amplitude( 150.0 );
  app->mean( 0.65 );
  app->sigma( 0.2 );
  app->asym( 3.0 );

  TCanvas *c = new TCanvas( "c1", "ESR", 794, 1123 );
  c->Divide( 1, 3 );
  c->cd( 1 );
  app->drawRho();

  c->cd( 2 );
  app->drawI( &esr );
  app->drawI( 316, 326.6 );

  c->cd( 3 );
  esr.GetGraph( true )->DrawClone( "Al" );

  
  // draw the density and intensity distributions.
  // The given esr data will be super imposed on the intensity distribution
  
  return 0;
}
