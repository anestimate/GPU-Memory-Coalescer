#pragma once

#include "Config.h"
#include <iostream>
#include <vector>
#include "WorkLoadGenerator.h"
#include "tb_util.h"


void workload_broadcast(const config& data, int bundle_count) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = generate_broadcast(data, bundle_count); 


    for (int i=0;i<bundle_count;i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);
    
    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 

void workload_coalesced_aligned(const config& data, int bundle_count) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = coalesced_aligned(data, bundle_count); 


    for (int i=0;i<bundle_count;i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);
    //output_json(cacheInstance);

    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 


void workload_coalesced_misaligned(const config& data, int bundle_count, int offset) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = coalesced_misaligned(data, bundle_count, offset); 


    for (int i=0;i<bundle_count;i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);
    //output_json(cacheInstance);

    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
}

void workload_strided_k(const config& data, int bundle_count, int stride) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = strided_k(data, bundle_count, stride); 


    for (int i=0;i<bundle_count;i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);

    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 

void workload_reverse(const config& data, int bundle_count) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = reverse(data, bundle_count); 


    for (int i=0;i<bundle_count;i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);

    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 

void workload_transpose(const config& data, int rows, int coloums) {
    
    config d = data;
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = transpose(data, rows,coloums); 


    for (size_t i=0;i<address.size();i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);
    
    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 


void workload_tiled(const config& data, int rows, int coloums) {
    
    config d = data;
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = tiled(data, rows,coloums); 


    for (size_t i=0;i<address.size();i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);

    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 

void workload_random_uniform(const config& data, int bundle_count, int seed, int region_elements) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = random_uniform(data, bundle_count, seed, region_elements); 


    for (size_t i=0;i<address.size();i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);

    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 


void workload_gather(const config& data, int bundle_count, int seed, int region_elements, int local_region_elements, double locality) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = gather(data, bundle_count, seed, region_elements, local_region_elements, locality); 


    for (size_t i=0;i<address.size();i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);

    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 

void workload_hotspot(const config& data, int bundle_count, int seed, int region_elements) {
    
    config d = data;
    
    Cache cacheInstance(d);
    Bundle dataBundle; 

    std::vector<Bundle> address = hotspot(data, bundle_count, seed, region_elements); 


    for (size_t i=0;i<address.size();i++) {
        multiple_requests(d, address[i], cacheInstance);
    }
        
    cacheInstance.drain(d);


    output_json(d, cacheInstance, statsService);
    output_stats(d, cacheInstance, statsService);
} 