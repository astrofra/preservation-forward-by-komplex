#include <iostream>

#include "app/export_config.h"
#include "app/forward_app.h"
#include "app/saari_capture.h"
#include "app/maku_capture.h"

int main(int argc, char** argv) {
    forward_offline::ExportConfig config;
    const forward_offline::ParseStatus status =
        forward_offline::parse_export_config(argc, argv, config, std::cerr);

    if (status == forward_offline::ParseStatus::help) {
        return 0;
    }
    if (status != forward_offline::ParseStatus::ok) {
        return 1;
    }

    if (config.sequence_name == "saari-gsplat") {
        return forward_offline::export_saari_capture(config);
    }
    if (config.sequence_name == "maku-gsplat") {
        return forward_offline::export_maku_capture(config);
    }
    forward_offline::ForwardApp app(config);
    return app.run();
}
