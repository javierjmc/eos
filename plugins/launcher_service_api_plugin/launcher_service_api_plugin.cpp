/**
 *  @file
 *  @copyright defined in eos/LICENSE
 */
#include <eosio/launcher_service_api_plugin/launcher_service_api_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _launcher_service_api_plugin = app().register_plugin<launcher_service_api_plugin>();

class launcher_service_api_plugin_impl {
   public:
};

launcher_service_api_plugin::launcher_service_api_plugin():my(new launcher_service_api_plugin_impl()){}
launcher_service_api_plugin::~launcher_service_api_plugin(){}

void launcher_service_api_plugin::set_program_options(options_description&, options_description& cfg) {
   cfg.add_options()
         ("option-name", bpo::value<string>()->default_value("default value"),
          "Option Description")
         ;
}

void launcher_service_api_plugin::plugin_initialize(const variables_map& options) {
   ilog("launcher_service_api_plugin::plugin_initialize()");
   try {
      if( options.count( "option-name" )) {
         // Handle the option
      }
   }
   FC_LOG_AND_RETHROW()
}

#define CALL(api_name, call_name, INVOKE, http_response_code) \
{std::string("/v1/" #api_name "/" #call_name), \
   [&](string, string body, url_response_callback cb) mutable { \
          try { \
             if (body.empty()) body = "{}"; \
             INVOKE \
             cb(http_response_code, fc::variant(result)); \
          } catch (...) { \
             http_plugin::handle_exception(#api_name, #call_name, body, cb); \
          } \
       }}

#define INVOKE_R_R(call_name, in_param) \
     auto result = app().get_plugin<launcher_service_plugin>().call_name(fc::json::from_string(body).as<in_param>());

#define INVOKE_R(call_name) \
     auto result = app().get_plugin<launcher_service_plugin>().call_name();

#define INVOKE_V_R(api_handle, call_name, in_param) \
     app().get_plugin<launcher_service_plugin>().call_name(fc::json::from_string(body).as<in_param>()); \
     eosio::detail::wallet_api_plugin_empty result;

void launcher_service_api_plugin::plugin_startup() {
   // Make the magic happen
   ilog("launcher_service_api_plugin::plugin_startup() 5");

   app().get_plugin<http_plugin>().add_api({
      CALL(launcher, get_info, INVOKE_R_R(get_info, std::string), 200),
      CALL(launcher, get_account, INVOKE_R_R(get_account, launcher_service::get_account_param), 200),
      CALL(launcher, get_cluster_info, INVOKE_R_R(get_cluster_info, int), 200),
      CALL(launcher, launch_cluster, INVOKE_R_R(launch_cluster, launcher_service::cluster_def), 200),
      CALL(launcher, stop_all_clusters, INVOKE_R(stop_all_clusters), 200),
      CALL(launcher, create_bios_accounts, INVOKE_R_R(create_bios_accounts, launcher_service::create_bios_accounts_param), 200),
      CALL(launcher, set_contract, INVOKE_R_R(set_contract, launcher_service::set_contract_param), 200),
      CALL(launcher, verify_transaction, INVOKE_R_R(verify_transaction, launcher_service::verify_transaction_param), 200)
   });
}

void launcher_service_api_plugin::plugin_shutdown() {
   ilog("launcher_service_api_plugin::plugin_shutdown()");
}

#undef CALL

}
