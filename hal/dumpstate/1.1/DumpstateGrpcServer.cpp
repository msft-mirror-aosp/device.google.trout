/*
 * Copyright (C) 2020 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "DumpstateGrpcServer.h"
#include "ServiceDescriptor.h"

#include <array>
#include <iostream>
#include <unordered_set>

#include <grpc++/grpc++.h>

struct GrpcServiceOutputConsumer : public ServiceDescriptor::OutputConsumer {
    using Dest = ::grpc::ServerWriter<dumpstate_proto::DumpstateBuffer>*;

    explicit GrpcServiceOutputConsumer(Dest s) : stream(s) {}

    void Write(char* ptr, size_t len) override {
        dumpstate_proto::DumpstateBuffer dumpstateBuffer;
        dumpstateBuffer.set_buffer(ptr, len);
        stream->Write(dumpstateBuffer);
    }

    Dest stream;
};

DumpstateGrpcServer::DumpstateGrpcServer(const std::string& addr, const ServiceSupplier& services)
    : mServiceAddr(addr) {
    mSystemLogsService = services.GetSystemLogsService();
    for (auto svc : services.GetServices()) {
        mServices.emplace(svc.name(), svc);
    }
}

static std::shared_ptr<::grpc::ServerCredentials> getServerCredentials() {
    // TODO(chenhaosjtuacm): get secured credentials here
    return ::grpc::InsecureServerCredentials();
}

grpc::Status DumpstateGrpcServer::GetSystemLogs(
        ::grpc::ServerContext*, const ::google::protobuf::Empty*,
        ::grpc::ServerWriter<dumpstate_proto::DumpstateBuffer>* stream) {
    GrpcServiceOutputConsumer consumer(stream);

    if (mSystemLogsService) {
        const auto ok = mSystemLogsService->GetOutput(&consumer);
        if (ok == std::nullopt)
            return ::grpc::Status::OK;
        else
            return ::grpc::Status(::grpc::StatusCode::INTERNAL, *ok);
    } else {
        return ::grpc::Status(::grpc::StatusCode::INTERNAL, "system logs missing");
    }
}

grpc::Status DumpstateGrpcServer::GetAvailableServices(
        ::grpc::ServerContext*, const ::google::protobuf::Empty*,
        dumpstate_proto::ServiceNameList* serviceList) {
    const dumpstate_proto::ServiceNameList kProtoAvailableServices = [this]() {
        dumpstate_proto::ServiceNameList serviceNameList;
        for (auto& svc : mServices) {
            if (svc.second.IsAvailable()) serviceNameList.add_service_names(svc.first);
        }
        return serviceNameList;
    }();

    *serviceList = kProtoAvailableServices;
    return ::grpc::Status::OK;
}

grpc::Status DumpstateGrpcServer::GetServiceLogs(
        ::grpc::ServerContext*, const dumpstate_proto::ServiceLogRequest* request,
        ::grpc::ServerWriter<dumpstate_proto::DumpstateBuffer>* stream) {
    const auto& serviceName = request->service_name();
    if (serviceName.empty()) {
        return ::grpc::Status::OK;
    }
    auto iter = mServices.find(serviceName);
    if (iter == mServices.end()) {
        return ::grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT,
                              std::string("Bad service name: ") + serviceName);
    }
    GrpcServiceOutputConsumer consumer(stream);
    const auto ok = iter->second.GetOutput(&consumer);
    if (ok == std::nullopt)
        return ::grpc::Status::OK;
    else
        return ::grpc::Status(::grpc::StatusCode::INTERNAL, *ok);
}

void DumpstateGrpcServer::Start() {
    ::grpc::ServerBuilder builder;
    builder.RegisterService(this);
    builder.AddListeningPort(mServiceAddr, getServerCredentials());
    std::unique_ptr<::grpc::Server> server(builder.BuildAndStart());

    if (!server) {
        std::cerr << __func__ << ": failed to create the GRPC server, "
                  << "please make sure the configuration and permissions are correct" << std::endl;
        std::abort();
    }

    server->Wait();
}
