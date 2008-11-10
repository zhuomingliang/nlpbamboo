# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl bamboo.t'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use Test::More tests => 3;
BEGIN { use_ok('bamboo') };

#########################

# Insert your test code below, the Test::More module is use()ed here so read
# its man page ( perldoc Test::More ) for help writing this test script.
my $handle = bamboo::init("");
ok($handle, "Bamboo Init");
my $result = bamboo::parse($handle, "你好朋友");
ok($result, "Bamboo Parse");
bamboo::clean($handle);
1;
