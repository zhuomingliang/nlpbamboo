#include "prepare_processor.hxx"
#include "token_impl.hxx"
#include <cstddef>

class PrepareProcessorTester : public PrepareProcessor {
public:
    PrepareProcessorTester(IConfig *config) : PrepareProcessor(config) {}
    void process_token(TokenImpl* token, std::vector<TokenImpl*> &out) {
        _process(token, out);
    }
}

bool test_concat_hyphen(PrepareProcessorTester processor) {
   TokenImpl input("");
   std::vector<TokenImpl*> output;
   processor.process_token(input, output);
}

bool test_ps_ident(PrepareProcessorTester processor) {
}
bool test_ps_whitespace(PrepareProcessorTester processor) {
}
int main() {
    SimpleConfig config();
    config["prepare_characterize"] = "1";
    config["concat_hyphen"] = "1";
    PrepareProcessorTester prepare_processor(config);
    bool concat_hyphen_test_passed = test_concat_hyphen(prepare_processor);
    bool ps_ident_test_passed = test_ps_ident(prepare_processor);
    bool ps_whitespace_passed = test_ps_whitespace(prepare_processor);
    return concat_hypen_test_passed 
            && ps_ident_test_passed
            && ps_whitespace_test_passed ? EXIT_SUCCESS : EXIT_FAILURE;
