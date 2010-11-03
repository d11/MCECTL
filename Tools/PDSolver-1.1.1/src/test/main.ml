
Progress_out.set_display false;
print_string "Beginning tests . . .\n\n";
Test_random.test_random ();
Test_pds_builder.test_pds_builder ();
Test_ma.test_ma ();
Test_state.test_state ();
Test_pds.test_pds ();
Test_parity_game.test_parity_game ();
Test_mc_parity.test_mc_parity ();
Test_mc_aux.test_mc_aux ();
Test_mc_mucalc.test_mc_mucalc ();
Test_mucalc.test_mucalc ();
print_string "Tests complete.\n\n";;


