/* Warning: This file is generated by ./collate-test - do not modify directly! */

    if ((properties&WRITABLE)&&!(properties&INMEMORY)) {
	static const test_desc tests[] = {
	    { "randomidx1", test_randomidx1 },
	    { 0, 0 }
	};
	result = max(result, test_driver::run(tests));
    }
    if ((properties&WRITABLE)&&!(properties&REMOTE)&&!(properties&INMEMORY)) {
	static const test_desc tests[] = {
	    { "valuesetmatchdecider1", test_valuesetmatchdecider1 },
	    { "alldocsiter1", test_alldocsiter1 },
	    { 0, 0 }
	};
	result = max(result, test_driver::run(tests));
    }
