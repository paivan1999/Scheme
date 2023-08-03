extern "C" const char* __asan_default_options() {  // NOLINT
    return "detect_leaks=0";
}
