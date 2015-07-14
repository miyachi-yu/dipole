/* ----------------------------------------------------------------
   file:         sample1.cc
   description: 
   a simple example to configure the density base distribution 
   and to draw resulting intensity distribution.

   Details of MyApplication class ( and its methods ) can be found
   in a header file, MyApplication.hh.

   ---------------------------------------------------------------- */
int sample1(){

  MyApplication *app = MyApplication::instance();
  
  // set offset value in t, resulting intensity distribution will be
  // centered at the given value, 300 mT.
  app->toffset( 300.0 );
  
  app->mean( 1.0 );        // set mean of asymmetric gaussian
  app->sigma( 0.1 );       // set sigma of asymmetric gaussian
  app->asym( 10.0 );       // set asymmetry value

  
  app->draw();       // draw the density and intensity distributions

  return 0;
}
