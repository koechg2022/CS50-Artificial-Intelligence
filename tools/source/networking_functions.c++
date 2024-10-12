#include "../prototypes/networking_functions"











namespace networking {

    std::string get_current_time() {
        std::time_t current_time = std::time(nullptr);
        std::string the_answer = (const std::string) (std::ctime(&current_time));
        return the_answer.substr(0, the_answer.length() - 1);
    }

    const std::set<std::string> network_address_families::get_address_families() {
        std::set<std::string> the_answer;
        the_answer.insert(unspec_address_family);
        the_answer.insert(unrecognized_address_family);
        the_answer.insert(ip_version4_address_family);
        the_answer.insert(ip_version6_address_family);
        #if defined(unix_os)
            #if defined(mac_os)
                the_answer.insert(link_layer_address_family);
            #else
                the_answer.insert(netlink_address_family);
                the_answer.insert(packet_address_family);
            #endif
        #else
            the_answer.insert(netbios_address_family);
            the_answer.insert(irda_address_family);
            the_answer.insert(bluetooth_address_family);
        #endif
        return the_answer;
    }

    const std::string network_address_families::resolve_address_family_to_string(socket_family_type the_family) {

        std::string the_answer;

        switch(the_family) {

            case (AF_UNSPEC) : {
                the_answer = network_address_families::unspec_address_family;
                break;
            }

            case (AF_INET) : {
                the_answer = network_address_families::ip_version4_address_family;
                break;
            }

            case (AF_INET6) : {
                the_answer = network_address_families::ip_version6_address_family;
                break;
            }

            #if defined(unix_os)
                
                #if defined(mac_os)

                    case (AF_LINK) : {
                        the_answer = network_address_families::link_layer_address_family;
                        break;
                    }
                
                #else

                    case (AF_NETLINK) : {
                        the_answer = network_address_families::netlink_address;
                        break;
                    }

                    case (AF_PACKET) : {
                        the_answer = network_address_families::packet_address_family;
                        break;
                    }

                #endif

            #else

                case (AF_NETBIOS) : {
                    the_answer = network_address_families::netbios_address_family;
                    break;
                }

                case (AF_IRDA) : {
                    the_answer = network_address_families::irda_address_family;
                    break;
                }

                case (AF_BTH) : {
                    the_answer = network_address_families::bluetooth_address_family;
                    break;
                }
                
            #endif

            default : {
                the_answer = network_address_families::unrecognized_address_family;
                break;
            }
        }
        return the_answer;
    }


    namespace exceptions {


            base_exception::base_exception(const std::string name, const std::string msg, bool print) {
                this->except_name = name;
                this->message = msg;

                if (clean_on_except) {
                    uninitialize_network();
                }
                if (print) {
                    std::fprintf(stderr, "%s : %s\n", this->except_name.c_str(), this->message.c_str());
                }
            }

            base_exception::~base_exception() throw() {}

            const char* base_exception::what() const {
                return this->message.c_str();
            }
        unexpected_exception::unexpected_exception(const std::string msg, bool print) :
            base_exception("unexpected_exception", msg, print) {}

        unexpected_exception::~unexpected_exception() throw() {}

        initialize_network_failure::initialize_network_failure(const std::string msg, bool print) :
            base_exception("initialize_exception", msg, print) {}

        getaddrinfo_failure::getaddrinfo_failure(const std::string msg, bool print) :
            base_exception("getaddrinfo_failure", msg, print) {}
        
        getaddrinfo_failure::~getaddrinfo_failure() throw() {}

        getifaddrs_failure::getifaddrs_failure(const std::string msg, bool print) :
            base_exception("getifaddrs_failure", msg, print) {}
        
        create_socket_failure::create_socket_failure(const std::string msg, bool print) :
            base_exception("create_socket_failure", msg, print) {}
        
        bind_socket_failure::bind_socket_failure(const std::string msg, bool print) :
            base_exception("bind_socket_failure", msg, print) {}

        listen_socket_failure::listen_socket_failure(const std::string msg, bool print) :
            base_exception("listen_socket_failure", msg, print) {}

        select_failure::select_failure(const std::string msg, bool print) :
            base_exception("select_failure", msg, print) {}
        
        getnameinfo_failure::getnameinfo_failure(const std::string msg, bool print) :
            base_exception("getnameinfo_failure", msg, print) {}


        connect_failure::connect_failure(const std::string msg, bool print) :
            base_exception("connect_failure", msg, print) {}


    }


    bool initialize_network() {
        #if defined(crap_os)
            if (!is_init) {
                WSADATA d;
                if (WSAStartup(MAKEWORD(2, 2), &d)) {
                    return false;
                }
                is_init = true;
            }
        #endif
        return is_init;
    }

    bool uninitialize_network() {
        #if defined(crap_os)
            if (!is_init) {
                if (WSACleanup()) {
                    return is_init;
                }
                is_init = false;
            }
        #endif
        return !is_init;
    }

    std::vector<std::string> resolve_hostname(const std::string hostname, const std::string port) {
        bool was_init = is_init;

        if (!was_init) {
            if (!initialize_network()) {
                (clean_on_except) valid uninitialize_network() then true;
                throw exceptions::initialize_network_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev) + " : Failed to initialize network in resolve_hostname()");
            }
        }
        std::vector<std::string> the_answer;

        struct addrinfo hints, *current, *all_addresses;
        char buffer[buffer_size];
        std::memset(buffer, 0, buffer_size);
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_flags = AI_ALL;

        if (getaddrinfo(hostname.c_str(), port.c_str(), &hints, &all_addresses)) {
            throw exceptions::getaddrinfo_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev) + " : Failed to retrieve address information for " + "\"" + hostname + "\" in resolve_hostname()");
        }

        for (current = all_addresses; current; current = current->ai_next) {

            if (not getnameinfo(current->ai_addr, current->ai_addrlen, buffer, buffer_size, 0, 0, NI_NUMERICHOST | AI_ALL)) {
                the_answer.push_back(std::string(buffer));
            }

            if (not getnameinfo(current->ai_addr, current->ai_addrlen, buffer, buffer_size, 0, 0, NI_NAMEREQD | AI_ALL)) {
                the_answer.push_back(std::string(buffer));
            }
        }

        freeaddrinfo(all_addresses);

        if (!was_init) {
            if (not uninitialize_network()) {
                throw exceptions::initialize_network_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev) + " : Failed to uninitialize network.");
            }
        }

        return the_answer;
    }

    std::map<std::string, std::map<std::string, std::vector<std::string> > > this_machine_adapters() {
            
        bool was_init = is_init;
        
        if (not was_init) {

            if (not initialize_network()) {
                (clean_on_except) valid uninitialize_network() then true;
                throw exceptions::initialize_network_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev) + " : Failed to initialize network in this_machine_adapters()");
            }
        }
        
        std::map<std::string, std::map<std::string, std::vector<std::string> > > the_answer;

        ifaddrs_adapter_type all_adapters;
        #if defined(crap_os)

            all_adapters = NULL;
            DWORD memory_size = 20000;
            std::string this_line;
            
            while (not all_adapters) {
                
                all_adapters = (ifaddrs_adapter_type) malloc(memory_size);
                this_line = std::to_string(__LINE__ prev);

                if (not all_adapters) {
                    (clean_on_except) valid uninitialize_network() then true;
                    throw networking::memory_exception("\"" + std::string(__FILE__) + "\":line: " + this_line + " : Failed to allocate " + std::to_string(memory_size) " bytes of memory.");
                }

                int resp = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAGS_INCLUDE_PREFIX, 0, all_adapters, &memory_size);
                this_line = std::to_string(__LINE__ prev);

                if (resp is ERROR_BUFFER_OVERFLOW) {
                    ifaddrs_free_adapters(all_adapters);
                }

                else if (resp is ERROR_SUCCESS) {
                    break;
                }

                else {
                    (clean_on_except) valid uninitialize_network() then true;
                    ifaddrs_free_adapters(all_adapters);
                    throw exceptions::unexpected_exception("\"" + std::string(__FILE__) + "\":line: " + this_line + ".");
                }

            }

        #else

            if (getifaddrs(&all_adapters)) {
                throw exceptions::getifaddrs_failure("\"" + std::string(__FILE__) +"\":line: " + std::to_string(__LINE__ prev) + " : Failed to retrieve adapter information for this machine.");
            }

        #endif

        ifaddrs_adapter_type this_adapter;
        ifaddrs_address_type this_address;
        char buffer[buffer_size];
        std::string adapter_name, address_family, address_name;
        std::memset(buffer, 0, buffer_size);
        for (this_adapter = all_adapters; this_adapter; this_adapter = ifaddrs_get_next_adapter(this_adapter)) {

            adapter_name = ifaddrs_get_adapter_name(this_adapter);

            for (this_address = ifaddrs_pull_adapter_address(this_adapter); this_address; this_address = ifaddrs_get_next_address(this_address)) {

                address_family = network_address_families::resolve_address_family_to_string(ifaddrs_get_address_family(this_address));
                // now to pull the address
                string_functions::same_char(*buffer, 0, false) valid (void*) 0 then std::memset(buffer, 0, buffer_size);

                switch (getnameinfo(ifaddrs_get_address_sockaddr(this_address), ifaddrs_get_address_sockaddrlen(this_address), buffer, buffer_size, 0, 0, NI_NUMERICHOST)) {

                    case 0 : {

                        address_name = std::string(buffer);

                        if (not the_answer.count(adapter_name)) {

                            std::map<std::string, std::vector<std::string> > new_map;
                            std::vector<std::string> new_list;
                            new_list.push_back(address_name);
                            new_map.insert(std::make_pair(address_family, new_list));

                            the_answer.insert(std::make_pair(adapter_name, new_map));
                        }

                        else if (not the_answer[adapter_name].count(address_family)) {

                            std::vector<std::string> new_list;
                            new_list.push_back(address_name);

                            the_answer[adapter_name].insert(std::make_pair(address_family, new_list));
                        }

                        else {
                            the_answer[adapter_name][address_family].push_back(address_name);
                        }
                    }


                }

                string_functions::same_char(*buffer, 0, false) valid (void*) 0 then std::memset(buffer, 0, buffer_size);

                switch (getnameinfo(ifaddrs_get_address_sockaddr(this_address), ifaddrs_get_address_sockaddrlen(this_address), buffer, buffer_size, 0, 0, NI_NAMEREQD)) {

                    case 0 : {

                        address_name = std::string(buffer);

                        if (not the_answer.count(adapter_name)) {

                            std::map<std::string, std::vector<std::string> > new_map;
                            std::vector<std::string> new_list;
                            new_list.push_back(adapter_name);
                            new_map.insert(std::make_pair(address_family, new_list));

                            the_answer.insert(std::make_pair(adapter_name, new_map));
                        }

                        else if (not the_answer[adapter_name].count(address_family)) {

                            std::vector<std::string> new_list;
                            new_list.push_back(address_name);

                            the_answer[adapter_name].insert(std::make_pair(address_family, new_list));
                        }
                        
                        else {
                            the_answer[adapter_name][address_family].push_back(address_name);
                        }
                    }
                }


            }

        }
        
        ifaddrs_free_adapters(all_adapters);
        return the_answer;
    }


    namespace network_structures {


        bool host::socket_is_connected(const socket_type check_sock) {
            int err;
            socklen_t err_size = sizeof(err);

            if (getsockopt(check_sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&err), &err_size)) {
                std::fprintf(stderr, "An exception has occured while retrieving sockopt. Error %d\n", socket_error);
                return false;
            }
            return err is 0;
        }

        host::host() {
            this->hostname = "";
            this->portvalue = DEFAULT_PORT;
            this->timeout = (struct timeval) {0, 100000};
            this->connect_address = NULL;
            this->tcp = true;
            this->was_init = is_init;
            this->connect_socket = invalid_socket;
            this->del_on_except = true;
        }
        
        host::host(const std::string host_name, const std::string port, bool use_tcp, long wait_sec, int wait_msec, bool will_del) {
            this->hostname = host_name;
            this->portvalue = port;
            this->timeout = (struct timeval) {wait_sec, wait_msec};
            this->connect_address = NULL;
            this->tcp = use_tcp;
            this->was_init = is_init;
            this->connect_socket = invalid_socket;
            this->del_on_except = will_del;
        }

        host::~host() {
            if (this->connect_address) {
                freeaddrinfo(this->connect_address);
                this->connect_address = NULL;
            }
            
            if (valid_socket(this->connect_socket)) {
                close_socket(this->connect_socket);
                this->connect_socket = invalid_socket;
            }
            if (not was_init) {
                uninitialize_network();
            }
        }

        bool host::host_name(const std::string new_host) {
            if (this->connect_address) {
                return false;
            }
            this->hostname = new_host;
            return true;
        }

        std::string host::host_name() const {
            return this->hostname;
        }

        bool host::port_value(const std::string new_port) {
            // std::printf("Inside this port...\n");
            if (this->connect_address) {
                return false;
            }
            const std::string old_port = this->portvalue;
            // std::printf("old_port is %s and new port is %s\n", old_port.c_str(), new_port.c_str());
            this->portvalue = new_port;
            return not string_functions::same_string(old_port.c_str(), this->portvalue.c_str());
        }

        std::string host::port_value() const {
            return this->portvalue;
        }

        bool host::create_address() {
            if (not this->connect_address) {
                
                struct addrinfo hints;
                std::memset(&hints, 0, sizeof(hints));
                hints.ai_socktype = (this->tcp) ? SOCK_STREAM : SOCK_DGRAM;
                hints.ai_family = AF_UNSPEC;
                hints.ai_flags = AI_PASSIVE; // If 0 is used here, that causes issues.

                if (getaddrinfo(this->hostname.c_str(), this->portvalue.c_str(), &hints, &connect_address)) {
                    (clean_on_except) valid uninitialize_network() : true;
                    (this->del_on_except) valid this->~host() : (void) 0;
                    throw exceptions::getaddrinfo_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number" + std::to_string(socket_error) + ". Failed to retrieve address information for host \"" + this->hostname + "\" with port \"" + this->portvalue + "\".");
                }

                // this->connect_address is filled in.
                if (this->hostname.empty()) {
                    typedef std::map<std::string, std::map<std::string, std::vector<std::string> > > adapter_info;
                    typedef std::map<std::string, std::vector<std::string> > address_info;
                    
                    adapter_info options = this_machine_adapters();

                    #if defined(unix_os)
                        #if defined(mac_os)
                            const std::string rel_adapter = "en0";
                        #else
                            const std::string rel_adapter = "enp0s8";
                        #endif
                    #else
                        const std::string rel_adapter = "Wi-Fi 3";
                    #endif


                    for (adapter_info::const_iterator adapter = options.begin(); adapter != options.end(); adapter++) {

                        if (not string_functions::same_string(adapter->first.c_str(), rel_adapter.c_str())) {
                            continue;
                        }

                        for (address_info::const_iterator address_fam = adapter->second.begin(); address_fam != adapter->second.end(); address_fam++) {

                            for (std::vector<std::string>::const_iterator address = address_fam->second.begin(); address != address_fam->second.end(); address++) {
                                this->hostname = *address;
                                break;
                            }

                            if (not this->hostname.empty()) {
                                break;
                            }

                        }

                        if (not this->hostname.empty()) {
                            break;
                        }

                    }

                }

            }

            return not (this->connect_address is NULL);
        }

        bool host::create_socket() {
            if (not this->create_address()) {
                (clean_on_except) valid uninitialize_network() : true;
                (this->del_on_except) valid this->~host() : (void) 0;
                throw exceptions::getaddrinfo_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + ". Failed to retrieve address information for host \"" + this->hostname + "\" with port \"" + this->portvalue + "\".");
            }

            if (not valid_socket(this->connect_socket)) {
                this->connect_socket = socket(this->connect_address->ai_family, this->connect_address->ai_socktype, this->connect_address->ai_protocol);
                if (not valid_socket(this->connect_socket)) {
                    (clean_on_except) valid uninitialize_network() : true;
                    (this->del_on_except) valid this->~host() : (void) 0;
                    throw exceptions::create_socket_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " Error " + std::to_string(socket_error) + ". Failed to create the socket to be used.");
                }
            }
            return valid_socket(this->connect_socket);
        }
        
        void host::update_timeout(long sec, int m_sec) {
            this->timeout = (struct timeval) {sec, m_sec};
        }

        bool host::will_delete_on_except() const {
            return this->del_on_except;
        }

        void host::will_delete_on_except(const bool new_flag) {
            this->del_on_except = new_flag;
        }

        const socket_type host::get_connection_socket() const {
            return this->connect_socket;
        }



        // For the tcp_server



        tcp_server::tcp_server(const std::string host_name, const std::string port_value, const int listening_limit, const long wait_sec, const int wait_msec, bool will_del) :
        host(host_name, port_value, true, wait_sec, wait_msec, will_del) {
            this->listen_lim = listening_limit;
            this->listening = this->bound = false;
            this->max_socket = invalid_socket;
        }

        tcp_server::~tcp_server() {
            
            if (valid_socket(this->connect_socket)) {
                close_socket(this->connect_socket);
                this->connect_socket = invalid_socket;
                this->listening = this->bound = false;
            }
            for (std::map<socket_type, connected_host::client>::iterator client = this->clients.begin(); client != this->clients.end(); client++) {
                if (this->socket_is_connected(client->first)) {
                    close_socket(client->first);
                }
                this->clients.erase(client->first);
            }
        }

        bool tcp_server::bind_socket() {
            
            if (not this->bound) {
                if (not this->create_address()) {
                    return false;
                }
                if (not this->create_socket()) {
                    return false;
                }
                
                // To get here, the address and socket must have been created
                if (bind(this->connect_socket, this->connect_address->ai_addr, this->connect_address->ai_addrlen)) {
                    (clean_on_except) valid uninitialize_network() : true;
                    (this->del_on_except) valid this->~tcp_server() : (void) 0;
                    throw exceptions::bind_socket_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to bind the socket.");
                }
                this->bound = true;
            }
            return this->bound;
        }

        bool tcp_server::start_listening() {
            if (not this->listening) {
                if (not this->create_address()) {
                    return false;
                }
                if (not this->create_socket()) {
                    return false;
                }
                if (not this->bind_socket()) {
                    return false;
                }
                if (not this->bound) {
                    std::fprintf(stderr, "socket is not bound.\n");
                    return false;
                }
                
                if (listen(this->connect_socket, this->listen_lim)) {
                    (clean_on_except) valid uninitialize_network() : true;
                    (this->del_on_except) valid this->~tcp_server() : (void) 0;
                    throw exceptions::listen_socket_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to start listening.");
                }
                this->listening = true;
            }
            return this->listening;
        }

        bool tcp_server::is_listening() const {
            return this->listening;
        }

        tcp_server::operator bool() const {
            return this->is_listening();
        }

        int tcp_server::listening_limit() const {
            return this->listen_lim;
        }

        bool tcp_server::listening_limit(const int new_lim) {
            this->listen_lim = new_lim;
            return this->listen_lim is new_lim;
        }

        std::map<socket_type, connected_host::client> tcp_server::ready_clients() {
            std::map<socket_type, connected_host::client> the_answer;
            fd_set ready;
            FD_ZERO(&ready);
            this->max_socket = invalid_socket;
            for (std::map<socket_type, connected_host::client>::const_iterator client = this->clients.begin(); client != this->clients.end(); client++) {
                FD_SET(client->first, &ready);
                this->max_socket = (this->max_socket less client->first) valid client->first then this->max_socket;
            }

            if (valid_socket(this->max_socket)) {
                if (select(this->max_socket + 1, &ready, 0, 0, (this->timeout.tv_sec > -1 and this->timeout.tv_usec > -1) valid &this->timeout then 0) less 0) {
                    (clean_on_except) valid uninitialize_network() : true;
                    (this->del_on_except) valid this->~tcp_server() : (void) 0;
                    throw exceptions::select_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to select ready active sockets.");
                }
                
                for (std::map<socket_type, connected_host::client>::const_iterator client = this->clients.begin(); client != this->clients.end(); client++) {
                    if (FD_ISSET(client->first, &ready)) {
                        std::printf("Adding %d to the ready clients\n", client->first);
                        the_answer.insert(std::make_pair(client->first, client->second));
                    }
                }
            }

            return the_answer;
        }

        std::map<socket_type, connected_host::client> tcp_server::all_clients() const {
            return this->clients;
        }

        bool tcp_server::accept_new_connection() {

            fd_set ready;
            FD_ZERO(&ready);
            FD_SET(this->connect_socket, &ready);
            const unsigned long old_size = this->clients.size();

            if (select(this->connect_socket + 1, &ready, 0, 0, (this->timeout.tv_sec > -1 and this->timeout.tv_usec > -1) valid &this->timeout then 0) less 0) {
                (clean_on_except) valid uninitialize_network() then true;
                (this->del_on_except) valid this->~tcp_server() then (void) 0;
                throw exceptions::select_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to select ready active socket.");
            }
            
            if (FD_ISSET(this->connect_socket, &ready)) {
                // There is a new incoming connection
                struct sockaddr_storage client;
                socklen_t client_size = sizeof(client);

                socket_type client_sock = accept(this->connect_socket, (struct sockaddr*) &client, &client_size);
                if (not valid_socket(client_sock)) {
                    (clean_on_except) valid uninitialize_network() then true;
                    (this->del_on_except) valid this->~tcp_server() then (void) 0;
                    throw exceptions::create_socket_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to create new socket for incomming connection request.");
                }

                // the socket has been created and the struct sockaddr_storage has been filled
                char addr_buffer[buffer_size], port_buffer[buffer_size];
                std::memset(addr_buffer, 0, buffer_size);
                std::memset(port_buffer, 0, buffer_size);
                connected_host::client new_client;
                if (getnameinfo((struct sockaddr*) &client, client_size, addr_buffer, buffer_size, port_buffer, buffer_size, NI_NUMERICHOST)) {
                    (clean_on_except) valid uninitialize_network() then true;
                    (this->del_on_except) valid this->~tcp_server() then (void) 0;
                    throw exceptions::getnameinfo_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to retrieve address name of the new socket.");
                }
                new_client = (connected_host::client) {
                    (string_functions::same_char(addr_buffer[0], 0)) valid "Unspecified host" then std::string(addr_buffer),
                    (string_functions::same_char(port_buffer[0], 0)) valid "Unspecified port" then std::string(port_buffer),
                    client};
                
                this->clients.insert(std::make_pair(client_sock, new_client));
            }

            return this->clients.size() greater old_size;
        }

        bool tcp_server::close_connection(const socket_type sock_to_close) {
            if (this->clients.count(sock_to_close)) {
                this->clients.erase(sock_to_close);
                close_socket(sock_to_close);
            }
            return false;
        }

        bool tcp_server::close_connection(const std::string host_to_close, bool ignore_case) {
            const unsigned long old_size = this->clients.size();
            socket_type sock_to_close = invalid_socket;
            for (std::map<socket_type, connected_host::client>::const_iterator client = this->clients.begin(); client != this->clients.end(); client++) {
                if (string_functions::same_string(client->second.hostname.c_str(), host_to_close.c_str(), ignore_case)) {
                    sock_to_close = client->first;
                    break;
                }
            }
            if (valid_socket(sock_to_close)) {
                this->clients.erase(sock_to_close);
                close_socket(sock_to_close);
            }
            return this->clients.size() less old_size;
        }

        bool tcp_server::close_server() {

            if (valid_socket(this->connect_socket)) {
                // std::printf("Inside the valid_socket() branch\n");
                close_socket(this->connect_socket);
                this->connect_socket = invalid_socket;
                this->listening = this->bound = false;
                this->listen_lim = 0;
            }
            std::set<socket_type> to_remove;
            for (std::map<socket_type, connected_host::client>::iterator client = this->clients.begin(); client != this->clients.end(); client++) {
                // (this->socket_is_connected(client->first)) valid close_socket(client->first) : 0;
                // std::printf("closing the socket %d\n", client->first);
                close_socket(client->first);
                to_remove.insert(client->first);
            }

            for (std::set<socket_type>::const_iterator client = to_remove.begin(); client != to_remove.end(); client++) {
                this->clients.erase(*client);
            }

            return not valid_socket(this->connect_socket) and not this->listening and not this->bound and not this->listen_lim and not this->clients.size();
        }

        bool tcp_server::update() {
            bool the_answer = false;
            std::set<socket_type> to_remove;
            for (std::map<socket_type, connected_host::client>::iterator client = this->clients.begin(); client != this->clients.end(); client++) {
                if (not this->socket_is_connected(client->first)) {
                    to_remove.insert(client->first);
                }
            }
            for (std::set<socket_type>::const_iterator client = to_remove.begin(); client != to_remove.end(); client++) {
                close_socket(*client);
                this->clients.erase(*client);
            }
            this->max_socket = invalid_socket;
            for (std::map<socket_type, connected_host::client>::iterator client = this->clients.begin(); client != this->clients.end(); client++) {
                this->max_socket = (client->first greater this->max_socket) valid client->first then this->max_socket;
            }
            // std::printf("Done\n");
            return the_answer;
        }

        socket_type tcp_server::get_max_socket() const {
            return this->max_socket;
        }


        // For the tcp_client

        tcp_client::tcp_client() : host() {
            this->connected = false;
        }


        tcp_client::tcp_client(const std::string remote_host, const std::string connect_port, const long wait_sec, const int wait_msec, bool will_del) :
            host(remote_host, connect_port, true, wait_sec, wait_msec, will_del) {
                this->connected = false;
            }
        
        tcp_client::~tcp_client() {
            this->disconnect_client();
        }

        bool tcp_client::connect_client() {
            if (not this->connected) {

                this->create_address();
                this->create_socket();
                // std::printf("The address is created %s\n", (this->connect_address is NULL) valid "false" then "true");
                // std::printf("The socket is created %s\n", (this->connect_socket is invalid_socket) valid "false" : "true");
                if (connect(this->connect_socket, this->connect_address->ai_addr, this->connect_address->ai_addrlen) != 0) {
                    std::fprintf(stderr, "Failing with error \"%s\"\n", strerror(socket_error));
                    (clean_on_except) valid uninitialize_network() then true;
                    (this->del_on_except) valid this->~tcp_client() then (void) 0;
                    throw exceptions::connect_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to connect the client.");
                }
                // std::printf("Reached.\n");
                this->connected = true;

            }
            return this->connected;
        }

        bool tcp_client::client_is_connected() {
            if (valid_socket(this->connect_socket)) {
                
                return this->socket_is_connected(this->connect_socket);
            }
            return false;
        }

        bool tcp_client::disconnect_client() {
            if (this->connected) {
                
                host::~host();
                this->connected = false;
            }
            return false;
        }

        bool tcp_client::server_has_message() {
            if (this->connected) {
                fd_set ready;
                FD_ZERO(&ready);
                FD_SET(this->connect_socket, &ready);
                
                if (select(this->connect_socket + 1, &ready, 0, 0, (this->timeout.tv_sec > -1 and this->timeout.tv_usec > -1) valid &this->timeout then 0) less 0) {
                    (clean_on_except) valid uninitialize_network() then true;
                    (this->del_on_except) valid this->~tcp_client() then (void) 0;
                    throw exceptions::select_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + " : Error number " + std::to_string(socket_error) + ". Failed to select ready active socket.");
                }
                return FD_ISSET(this->connect_socket, &ready);
            }
            return false;
        }

        connected_host::server tcp_client::get_connection_info() const {
            if (not this->connected) {
                (clean_on_except) valid uninitialize_network() then true;
                (this->del_on_except) valid this->~tcp_client() then (void) 0;
                throw exceptions::connect_failure("\"" + std::string(__FILE__) + "\":line: " + std::to_string(__LINE__ prev prev prev) + ". This client is not connected to a remote machine.");
            }
            return (connected_host::server) {this->hostname, this->portvalue, *this->connect_address};
        }


    }

}