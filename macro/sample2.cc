int sample2(){

  MyApplication *app = MyApplication::instance();
  
  ESR esr( "cofeebean-a.txt" );

  app->amplitude( 150.0 );
  app->mean( 0.65 );
  app->sigma( 0.2 );
  app->asym( 3.0 );
  app->draw( &esr );

  return 0;
}
