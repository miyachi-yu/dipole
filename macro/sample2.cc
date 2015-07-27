/* ----------------------------------------------------------------
   file:         sample2.cc
   description: 
   Example for ESR data handling. Details of the ESR class can be
   found in Quark Group Wiki Page
   http://www.quark.kj.yamagata-u.ac.jp/dokuwiki/doku.php?id=quark:pt:esr

   esr2.zip was imported to this analsys software.
   ---------------------------------------------------------------- */
int sample2(){

  MyApplication *app = MyApplication::instance();
  
  ESR *esr = new ESR( "cofeebean-a.txt" ); // prepare ESR data from the given file

  app->amplitude( 150.0 );
  app->mean( 0.65 );
  app->sigma( 0.2 );
  app->asym( 3.0 );

  // draw the density and intensity distributions.
  // The given esr data will be super imposed on the intensity distribution
  app->draw( esr );

  return 0;
}
