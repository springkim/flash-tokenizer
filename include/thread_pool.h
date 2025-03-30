//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                               License Agreement
//                            ThreadPool4BatchEncoding
//
//               Copyright (C) 2025, Kim Bomm, all rights reserved.
//
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
#pragma once
#ifndef BRLKNPYZFSMVUGRMSQGMWRWAXYXTRUTYSRPKQTFEJCHGNVRPQCTXCEWNJGQZUV
#define BRLKNPYZFSMVUGRMSQGMWRWAXYXTRUTYSRPKQTFEJCHGNVRPQCTXCEWNJGQZUV

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    size_t num_threads;

    explicit ThreadPool(const size_t numThreads = 0) : stop(false) {
        // Optimize thread count based on platform and workload
        if (numThreads == 0) {
#ifdef _WIN32
            // On Windows, using fewer threads can reduce overhead
            // Use 75% of available cores to avoid oversubscription
            size_t hw_concurrency = std::thread::hardware_concurrency();
            this->num_threads = hw_concurrency > 2 ? (hw_concurrency * 3) / 4 : hw_concurrency;
#else
            // On other platforms, use all available cores
            this->num_threads = std::thread::hardware_concurrency();
#endif
        } else {
            this->num_threads = numThreads;
        }

        // Initialize worker threads
        for (size_t i = 0; i < this->num_threads; ++i)
            workers.emplace_back([this, i] {
                // Each thread processes tasks from the queue
                while (true) {
                    std::function<void()> task;
                    {
                        // Critical section - minimize time spent holding the lock
                        std::unique_lock<std::mutex> lock(queueMutex);
                        
                        // Wait for work or stop signal
                        condition.wait(lock, [this] { 
                            return stop || !tasks.empty(); 
                        });
                        
                        // Exit if stopped and no more tasks
                        if (stop && tasks.empty())
                            return;
                        
                        // Get task from queue
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    
                    // Execute task outside of the lock
                    task();
                }
            });
    }

    // Enqueue a single task
    template<class F, class... Args>
    auto enqueue(F &&f, Args &&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type> {
        using return_type = typename std::invoke_result<F, Args...>::type;
        
        // Create packaged task
        auto task = std::make_shared<std::packaged_task<return_type()> >(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        // Get future for result
        std::future<return_type> res = task->get_future();
        
        // Add task to queue
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.emplace([task]() { (*task)(); });
        }
        
        // Notify one worker
        condition.notify_one();
        return res;
    }

    // Batch enqueue for multiple similar tasks to reduce overhead
    template<class F, class Container>
    auto batch_enqueue(F &&f, const Container &items)
        -> std::vector<std::future<typename std::invoke_result<F, typename Container::value_type>::type>> {
        using item_type = typename Container::value_type;
        using return_type = typename std::invoke_result<F, item_type>::type;
        
        std::vector<std::future<return_type>> futures;
        futures.reserve(items.size());
        
        // Create all tasks at once to reduce lock contention
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            
            for (const auto &item : items) {
                auto task = std::make_shared<std::packaged_task<return_type()>>(
                    std::bind(std::forward<F>(f), item)
                );
                
                futures.push_back(task->get_future());
                tasks.emplace([task]() { (*task)(); });
            }
        }
        
        // Notify all workers at once
        condition.notify_all();
        return futures;
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker: workers)
            worker.join();
    }

private:
    std::vector<std::thread> workers{};
    std::queue<std::function<void()>> tasks{};
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
};

#endif
