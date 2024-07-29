import std; // Module import directive.


int run(void *) {
    while (true) {
        std::cout << "[Child] EUID/EUID "
            << geteuid()
            << "/" << geteuid()
            << std::endl;
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    [...]
    childPid = clone(run, childStackTop, CLONE_NEWUSER, 0);
    std::cout << "[Parent] Child PID: " << childPid << std::endl;
    sleep(5);

    // echo "0 1000 1" >> /proc/<PID>/uid_map
    std::string cmd = "echo '0 " +
                      std::to_string(getuid()) +
                      " 1' >> /proc/" +
                      std::to_string(childPid) +
                      "/uid_map";

    system(cmd.c_str());
    return 0;
