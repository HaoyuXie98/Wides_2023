//
// Copyright 2023, University of Southern California, WiDeS Group
//
// Author: 
//      - Yuning (Brian) Zhang
//      - Haoyu Xie
//      - Sai Sakala
//

#include "WiDeS_Func.hpp"



//// ====== Constants ======
    /*!
     * @param main_data_path the path that stops one layer above the dated folders.
     * @param Ctrl_PAAM_py_path the path that reaches PAAM actions.
     * @param Ctrl_Table_py_path the path that reaches XY Table actions.
     */
    const std::string main_data_path = "/root/uhd/host/examples/Brian/Data/";
    const std::string Ctrl_PAAM_py_path = "/root/Ctrl_PAAM_py/paam_actions/";
    const std::string Ctrl_Table_py_path = "/root/Ctrl_Table_py/xytable_actions/";



/// ====== Printing Template ======
    void printheader() {
        std::cout << ".........................................." << std::endl;
    }
    void printfoot() {
        std::cout << ".........................................." 
                    << std::endl
                    << std::endl
                    << std::endl;
    }



//// ====== Python API ======
    /*!
    * @brief Python API to call Python script in C++
    * 
    * @param name the name of the Python script along with its relative path
    * @param args all the arguments that need to be passed to the Python script, such as "-t 200 -i 10 -n 5"
    */
    int python_api(std::string name, std::string args) 
    {
        // Prepare the base command
        std::string command = "python " + name + " " + args;

        // Run the command
        int result = system(command.c_str());

        if (result != 0) {
            std::cout << "An error occurred while executing the Python script: " << command << std::endl;
            throw std::runtime_error("Python Execution Error");
        }

        std::ifstream file("output.txt");
        std::string line;
        std::map<std::string, std::string> data;
        while (std::getline(file, line)) {
            std::istringstream is_line(line);
            std::string key;
            if (std::getline(is_line, key, ',')) {
                std::string value;
                if (std::getline(is_line, value)) {
                    data[key] = value;
                }
            }
        }

        // Print the data
        std::cout << "return value:" << std::endl;
        for (const auto &pair : data) {
            std::cout << pair.first << ": " << pair.second << std::endl;
        }

        return 0;
    }



// //// ====== Send from File ======
//     /*!
//     * @brief Read the complex BB signal from the provided .txt file. 
//     *
//     * @param filename_read the name of the file that will read the data
//     * @param spb samples per buffer, length equals to a single rep of sounding signal
//     * @return The buffer that holds the data
//     */
//     std::vector<std::complex<float>> send_from_file(const std::string& filename_read, 
//                                                     size_t spb)
//     {
//         // --- define the buffer and file reader ---
//         std::vector<std::complex<float>> buff(spb);
//         std::ifstream signal_file(filename_read.c_str());

//         // --- read file ---
//         float real, imag;
//         size_t i = 0;
//         while (i < spb && signal_file >> real >> imag)
//         {
//             buff[i] = (std::complex<float>(real, imag));
//             ++i;
//         }

//         std::vector<std::complex<float>> original_buff = buff; // Save original buffer
//         for(int i = 0; i < 9; i++) { // Repeat 9 times to make total repetitions to be 10
//             buff.insert(buff.end(), original_buff.begin(), original_buff.end());
//         }

//         // close file
//         signal_file.close();

//         // --- return buffer ---
//         return buff;
//     }




//// ====== Wavetable ======
    /*!
     * @param filename_read the name of the file that will read the data
     */
    std::vector<std::complex<float>> cosmos_wavetable(const std::string filename_read)
    {
        size_t wavetable_len = 500;
        std::ifstream rowCounter(filename_read);    // counter of the rows of the signal file
        size_t rows = 0;                               // counted rows
        std::string line;
        if (rowCounter.is_open()){
            while (!rowCounter.eof()){
                std::getline(rowCounter, line);
                rows++;
            }
            rowCounter.close();
        }
        rows--;     // the number of rows after counting is +1, so subtract 1 for actual line amount

        std::ifstream reader(filename_read);        // reader of the actual data from the signal file
        double read_I, read_Q;
        std::vector<std::complex<float>> complex_wavetable(wavetable_len);
        // std::vector<float> separated_complex_wavetable(wave_table_len);    // a real-values wavetable that contains complex numbers as real-pairs
        // if (reader.is_open()){
        //     for (size_t ii = 0; ii < rows; ii+){
        //         reader >> read_I;
        //         for (size_t qq = 1; qq < 2; qq++){
        //             separated_complex_wavetable[ii] = read_I;
        //             reader >> read_Q;
        //             separated_complex_wavetable[qq] = read_Q;
        //             complex_wavetable[ii] = std::complex<float>(separated_complex_wavetable[ii], 
        //                                                         separated_complex_wavetable[qq]);
        //         }
        //     }
        // }
        if (reader.is_open()){
            for (size_t ii = 0; ii < rows; ii++){
                reader >> read_I >> read_Q;
                complex_wavetable[ii] = std::complex<float>(read_I, read_Q);
            }
        }
        return complex_wavetable;
    }




//// ====== Send from File ======
    /*!
    * @brief Read the complex BB signal from the provided .txt file. 
    *
    * @param filename_read the name of the file that will read the data
    * @param spb samples per buffer, length equals to a single rep of sounding signal
    * @return The buffer that holds the data
    */
    std::vector<std::complex<float>> send_from_file(const std::string filename_read, 
                                                    size_t spb)
    {
        // --- define the buffer and file reader ---
        std::vector<std::complex<float>> buff(spb);
        std::vector<std::complex<float>*> buffs(1, &buff.front());

        // --- load buffer from wavetable ---
        size_t wavetable_idx_tmp = -1;
        size_t wavetable_idx;
        const size_t wavetable_read_step = 1;
        std::vector<std::complex<float>> complex_wavetable = cosmos_wavetable(filename_read);
        for (size_t n = 0; n < buff.size(); n++) {
            wavetable_idx_tmp += wavetable_read_step;
            wavetable_idx = wavetable_idx_tmp % complex_wavetable.size();
            buff[n] = complex_wavetable[wavetable_idx];
        }
        // --- return buffer ---
        return buff;
    }





//// ====== Receive to File ======
    /*!
    * @brief One-time action to receive data at a USRP recv_to_file function - one time action
    * 
    * @param rx_usrp a USRP object
    * @param rx_channel the logic channel that will be used for the USRP daughterboard
    * @param filename_write the name of the file that will store the data
    * @param spb "samples per buffer", buffer size to save data (samples per buffer)
    * @param time_to_recv the "USRP time" that starts to receive after set a time using set_time...()
    * @param cpu_format data format when storing in USRP memory
    * @param wire_format data format during transmission in physical cables/lines - format description check Git Wiki
    */
    void recv_to_file(uhd::usrp::multi_usrp::sptr rx_usrp,      // a USRP object
        size_t              rx_channel,                         // channel of Rx port
        const std::string&  filename_write,                     // file name to save data
        size_t              spb,                                // buffer size to save data (samples per buffer)
        uhd::time_spec_t    time_to_recv,                       // the "USRP time" that starts to receive after set a time using set_time...()
        std::string         cpu_format,                         // data format when storing in USRP memory
        std::string         wire_format                         // data format during transmission in physical cables/lines - format description check Git Wiki
        )
    {
        // ====== Define Variables ======
            // system parameters
            size_t num_received_samps       = 0;        // number of samples have received so far
            size_t num_rx_samps_tmp         = 0;
            const double timeout            = 20.0;     // in sec

            // --- create a receive streamer ---
            uhd::stream_args_t stream_args(cpu_format, wire_format);
            std::vector<size_t> stream_args_channels;
            stream_args_channels.push_back(rx_channel);
            stream_args.channels = stream_args_channels;
            uhd::rx_streamer::sptr rx_stream = rx_usrp->get_rx_stream(stream_args);

            // Rx metadata
            uhd::rx_metadata_t rx_metadata;

            // define buffer
            std::vector<std::complex<float>> buff(spb);

            // Set up data writing
            std::ofstream data_file;
            char full_file_name[200];
            strcpy(full_file_name, filename_write.c_str());
            strcat(full_file_name, ".bin");
            data_file.open(full_file_name, std::ofstream::binary);

            // Set up metadata writing
            std::ofstream metadata_file;
            char full_metafile_name[200];

        // ====== Configure streaming ======
            uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_NUM_SAMPS_AND_DONE);    // streaming mode: continuous
            // Number of samples to receive
            stream_cmd.num_samps  = size_t(spb);  // number of samples to receive every time
            // time to receive samples
            stream_cmd.stream_now = false;  // do NOT set it as true - otherwise the synchronization status will be lost!
            stream_cmd.time_spec  = time_to_recv;   // when to stream (related to synchronization) - stream_now & time_spec
            // issue the streaming command
            rx_stream->issue_stream_cmd(stream_cmd);    // configure the USRP accordingly

        // ====== Start ======
            std::cout << std::endl << std::endl;
            printheader();
            std::cout << "Rx: capturing will start at t = " << time_to_recv.get_real_secs() << "..."
                << std::endl;
            

            const auto start_time = std::chrono::steady_clock::now();   // Mark the starting timestamp

        // ====== Receive and Write Data ======
            // Receive samples into the pre-assigned buffer
            num_rx_samps_tmp =
                rx_stream->recv(&buff.front(), buff.size(), rx_metadata, timeout);
            std::cout << "Will try to receive " << num_rx_samps_tmp << " samples. " << std::endl;
            printfoot();
            // Define error cases
                // - 1 - Time out
                if (rx_metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT) {
                    std::cout << boost::format("Timeout while streaming") << std::endl;
                    throw std::runtime_error("Timeout");
                }
                // - 2 - Overflow
                if (rx_metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_OVERFLOW) {
                    std::cerr
                        << boost::format(
                            "Got an overflow indication. Please consider the following:\n"
                            "  Your write medium must sustain a rate of %fMB/s.\n"
                            "  Dropped samples will not be written to the file.\n"
                            "  Please modify this example for your purposes.\n")
                            % (rx_usrp->get_rx_rate() * sizeof(std::complex<double>) / 1e6);
                    throw std::runtime_error("Overflow");
                }
                // - 3 - Other errors
                if (rx_metadata.error_code != uhd::rx_metadata_t::ERROR_CODE_NONE) {
                    std::string error = str(boost::format("Receiver error: %s") % rx_metadata.strerror());
                    throw std::runtime_error(error);
                }

            // update Rx sample counter
            num_received_samps += num_rx_samps_tmp;

            // write data to file from buffer
            data_file.write((const char*)&buff.front(), num_rx_samps_tmp * sizeof(std::complex<float>));       
            
            // Mark the ending timestamp
            const auto actual_stop_time = std::chrono::steady_clock::now();

        // ====== Wrap up ======
            // close files
            data_file.close();

            // shut down receiver
            stream_cmd.stream_mode = uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS;
            rx_stream->issue_stream_cmd(stream_cmd);       

            // print Rx finishes
            printheader();
            std::cout<< "Rx Done!" <<std::endl;
            

        // ====== Status check ======
            const double actual_duration_seconds =
                std::chrono::duration<float>(actual_stop_time - start_time).count();

            std::cout << boost::format("Received %d samples in %f seconds") % num_received_samps
                            % actual_duration_seconds
                    << std::endl;
            
            const double rate_checked = (double)num_received_samps / actual_duration_seconds;
            std::cout << "with rate of " << (rate_checked / 1e6) << " Msps" << std::endl;
            printfoot();

        // ====== Process Metadata ======
            long long rx_starting_tick = rx_metadata.time_spec.to_ticks(200e6);
            double rx_starting_sec = rx_metadata.time_spec.get_real_secs();
            printheader();
            std::cout << "Metadata Here... " << std::endl;
            std::cout << "  Streaming starting tick = " << rx_starting_tick << std::endl;
            std::cout << "  Streaming starting sec = " << rx_starting_sec 
                        << std::endl;
            printfoot();
            
            strcpy(full_metafile_name, filename_write.c_str());
            strcat(full_metafile_name, "_metadata.dat");
            metadata_file.open(full_metafile_name, std::ofstream::binary);
            metadata_file.write((char*)&rx_starting_tick, sizeof(long long));
            metadata_file.close();

            std::cout << "=========================" << std::endl;
            std::cout << "Data is saved in file: " << std::endl;
            std::cout << boost::format("%s") % full_file_name
                    << std::endl
                    << std::endl;
            std::cout << "Metadata is saved in file: " << std::endl;
            std::cout << boost::format("%s") % full_metafile_name
                    << std::endl;
            std::cout << "=========================" << std::endl;
            std::cout << std::endl;
    } // "recv_to_file()" ends



//// ====== Configure Tx USRP ======
    /*!
    * @brief Configuration of Tx USRP. Universal procedures with Rx
    * 
    * @param tx_ant the physical antenna port, meaning the exact SMA connector port on 
    *          the device; https://files.ettus.com/manual/page_dboards.html#dboards_ubx
    * @param tx_channel the logic channel within the device, you can select either one 
    *          of the two/four
    * @param tx_subdev the daughtorboard option, don't modify it if unnecessary. 
    *          For now keep it as B:channel
    * @param tx_args specify the USRP device, can be "addr=", 'mgmt_addr=', 'resource=', or others
    * @param pps the 1 PPS signal source; https://files.ettus.com/manual/classuhd_1_1usrp_1_1multi__usrp.html#a57a5580ba06d7d6a037c9ef64f1ea361
    * @param ref the 10 MHz reference signal source; https://files.ettus.com/manual/classuhd_1_1usrp_1_1multi__usrp.html#a73ed40009d0d3787c183d42423d25026
    * @param tx_rate the sampling rate, at baseband, in Samples/sec, should be 200 MHz
    * @param IF_freq the 1st-stage IF center frequency, in Hz. Should be 3 GHz; Then 
    *          PAAM will keep UPC from 3 GHz to 28 GHz. 
    * @param tx_gain Tx RF gain from the USRP
    */
    uhd::usrp::multi_usrp::sptr config_Tx_usrp(std::string tx_ant,
                                                size_t tx_channel,
                                                std::string tx_subdev,
                                                std::string tx_args,
                                                
                                                std::string pps,
                                                std::string ref,

                                                double tx_rate,
                                                double IF_freq,

                                                double tx_gain)
    {
        // --- create a usrp device ---
        std::cout << std::endl << std::endl;
        printheader();
        std::cout << boost::format("Creating the usrp device with: %s...") % tx_args
                << std::endl;
        uhd::usrp::multi_usrp::sptr tx_usrp = uhd::usrp::multi_usrp::make(tx_args);
        printfoot();


        // --- select subdevice (always use default value) ---
        tx_usrp->set_tx_subdev_spec(tx_subdev);

        // --- set timing ---
        tx_usrp->set_clock_source(ref); // ref as external
        tx_usrp->set_time_source(pps);  // pps as external
        printheader();
        std::cout<<boost::format("The reference clock source is: %s...") % ref
            <<std::endl;
        std::cout<<boost::format("The PPS signal source is: %s...") % pps
            <<std::endl;
        tx_usrp->set_time_unknown_pps(uhd::time_spec_t(0.0));   // set timestamp: https://files.ettus.com/manual/classuhd_1_1usrp_1_1multi__usrp.html#a413014bf3aea4a8ea2d268b4a3b390e9
        std::cout << boost::format("Device timestamp has been set to 0.")
            << std::endl;
        printfoot();

        // --- set sampling rate ---
        tx_usrp->set_tx_rate(tx_rate);    
        printheader();    
        std::cout << boost::format("TX sampling rate has been set to: %f Msps...") % (tx_usrp->get_tx_rate() / 1e6)
            << std::endl;

        // --- set IF tuning ---
        uhd::tune_request_t tune_request(IF_freq);  // specify the frequency
        tune_request.args = uhd::device_addr_t("mode_n=integer");   // tune the USRP with integer-N mode
        tx_usrp->set_tx_freq(tune_request, tx_channel);
        std::cout << boost::format("TX IF frequency has been set to: %f MHz...") % (tx_usrp->get_tx_freq(tx_channel) / 1e6)
            << std::endl;

        // --- set RF gain ---
        tx_usrp->set_tx_gain(tx_gain, tx_channel);
        std::cout << boost::format("TX gain has been set to: %f dB...") % tx_usrp->get_tx_gain(tx_channel)
            << std::endl;
        printfoot();

        // --- set the antenna ---
        tx_usrp->set_tx_antenna(tx_ant, tx_channel);

        // --- consolidate configuration
        std::this_thread::sleep_for(std::chrono::seconds(1)); // allow for some setup time

        // --- checking LO and Ref locking status ---
        std::vector<std::string> sensor_names;
        sensor_names = tx_usrp->get_tx_sensor_names(tx_channel); // get the Tx sensor names for channel: tx_channel
            /* Available Tx sensors:
            * lo_locked
            */
        printheader();
        if (std::find(sensor_names.begin(), sensor_names.end(), "lo_locked")    // if "lo_locked" sensor is detected
            != sensor_names.end()) {
            uhd::sensor_value_t lo_locked = tx_usrp->get_tx_sensor("lo_locked", tx_channel);
            std::cout << boost::format("TX LO locking status: %s. ") % lo_locked.to_pp_string()
                    << std::endl;
            UHD_ASSERT_THROW(lo_locked.to_bool());
        }

        sensor_names = tx_usrp->get_mboard_sensor_names(0); // there is only 1 motherboard in this USRP so the index of it is: 0
            /* Available motherboard sensors:
            * gps_gpgga
            * gps_gprmc
            * gps_time
            * gps_locked
            * gps_servo
            * ref_locked
            */
                
        if (std::find(sensor_names.begin(), sensor_names.end(), "ref_locked") != sensor_names.end()) {    // if "ref_locked" sensor is detected
            uhd::sensor_value_t ref_locked = tx_usrp->get_mboard_sensor("ref_locked", 0);
            std::cout << boost::format("TX reference locking status: %s. ") % ref_locked.to_pp_string()
                    << std::endl;
            UHD_ASSERT_THROW(ref_locked.to_bool());
        }
        printfoot();

        

        // --- print overall USRP info ---
        // std::cout << boost::format("TX USRP Device Info: %s") % tx_usrp->get_pp_string() << std::endl;
        std::cout << "=========================" << std::endl; 
        std::cout << "Configuration Success!" << std::endl;
        std::cout << "=========================" << std::endl << std::endl;
        // --- return object ---
        return tx_usrp;
    }



//// ====== Configure Rx USRP ======
    /*!
    * @brief Configuration of Rx USRP. Universal procedures with Tx
    * 
    * @param rx_ant the physical antenna port, meaning the exact SMA connector port on 
    *          the device; https://files.ettus.com/manual/page_dboards.html#dboards_ubx
    * @param rx_channel the logic channel within the device, you can select either one 
    *          of the two/four
    * @param rx_subdev section "Subdev Specifications" & "N310-specific Features" of: 
    *          https://files.ettus.com/manual/page_usrp_n3xx.html#n3xx_feature_list_mg
    * @param rx_args specify the USRP device, can be "addr=", 'mgmt_addr=', 'resource=', or others
    * @param pps the 1 PPS signal source; https://files.ettus.com/manual/classuhd_1_1usrp_1_1multi__usrp.html#a57a5580ba06d7d6a037c9ef64f1ea361
    * @param ref the 10 MHz reference signal source; https://files.ettus.com/manual/classuhd_1_1usrp_1_1multi__usrp.html#a73ed40009d0d3787c183d42423d25026
    * @param rx_rate the sampling rate, at baseband, in Samples/sec, should be 200 MHz
    * @param IF_freq the 1st-stage IF center frequency, in Hz. Should be 3 GHz; Then 
    *          PAAM will do DWC from 28 GHz to 3 GHz. 
    * @param rx_gain Rx RF gain from the USRP
    */
    uhd::usrp::multi_usrp::sptr config_Rx_usrp(std::string rx_ant,
                                                size_t rx_channel,
                                                std::string rx_subdev,
                                                std::string rx_args,
                                                
                                                std::string pps,
                                                std::string ref,

                                                double rx_rate,
                                                double IF_freq,

                                                double rx_gain)
    {
        // --- create a usrp device ---
        std::cout << std::endl << std::endl;
        printheader();
        std::cout << boost::format("Creating the usrp device with: %s...") % rx_args
                << std::endl;
        uhd::usrp::multi_usrp::sptr rx_usrp = uhd::usrp::multi_usrp::make(rx_args);

        // --- select subdevice (always use default value) ---
        rx_usrp->set_rx_subdev_spec(rx_subdev);
        printfoot();

        // --- set timing ---
        rx_usrp->set_clock_source(ref); // ref as external
        rx_usrp->set_time_source(pps);  // pps as external
        printheader();
        std::cout<<boost::format("The reference clock source is: %s...") % ref
            <<std::endl;
        std::cout<<boost::format("The PPS signal source is: %s...") % pps
            <<std::endl;
        rx_usrp->set_time_unknown_pps(uhd::time_spec_t(0.0));   // set timestamp: https://files.ettus.com/manual/classuhd_1_1usrp_1_1multi__usrp.html#a413014bf3aea4a8ea2d268b4a3b390e9
        std::cout << boost::format("Device timestamp has been set to 0.")
            << std::endl;
        printfoot();

        // --- set sampling rate ---
        rx_usrp->set_rx_rate(rx_rate);   
        printheader();        
        std::cout << boost::format("RX sampling rate has been set to: %f Msps...") % (rx_usrp->get_rx_rate() / 1e6)
            << std::endl;

        // --- set IF tuning ---
        uhd::tune_request_t tune_request(IF_freq);  // specify the frequency
        tune_request.args = uhd::device_addr_t("mode_n=integer");   // tune the USRP with integer-N mode
        rx_usrp->set_rx_freq(tune_request, rx_channel);
        std::cout << boost::format("RX IF frequency has been set to: %f MHz...") % (rx_usrp->get_rx_freq(rx_channel) / 1e6)
            << std::endl;

        // --- set RF gain ---
        rx_usrp->set_rx_gain(rx_gain, rx_channel);
        std::cout << boost::format("RX gain has been set to: %f dB...") % rx_usrp->get_rx_gain(rx_channel)
            << std::endl;
        printfoot();

        // --- set the antenna ---
        rx_usrp->set_rx_antenna(rx_ant, rx_channel);

        // --- consolidate configuration
        std::this_thread::sleep_for(std::chrono::seconds(1)); // allow for some setup time

        // --- checking LO and Ref locking status ---
        std::vector<std::string> sensor_names;
        sensor_names = rx_usrp->get_rx_sensor_names(rx_channel); // get the Rx sensor names for channel: rx_channel
                /* Available Rx sensors:
                * lo_locked
                * ad9371_lo_locked
                * lowband_lo_locked
                */
        printheader();
        if (std::find(sensor_names.begin(), sensor_names.end(), "lo_locked")    // if "lo_locked" sensor is detected
            != sensor_names.end()) {
            uhd::sensor_value_t lo_locked = rx_usrp->get_rx_sensor("lo_locked", rx_channel);
            std::cout << boost::format("RX LO locking status: %s ...") % lo_locked.to_pp_string()
                    << std::endl;
            UHD_ASSERT_THROW(lo_locked.to_bool());
        }

        sensor_names = rx_usrp->get_mboard_sensor_names(0); // there is only 1 motherboard in this USRP so the index of it is: 0
                /* Available motherboard sensors:
                * ref_locked
                * gps_gpgga
                * gps_time
                * gps_locked
                * tmp
                * gps_tpv
                * fan
                * gps_sky
                */
        if (std::find(sensor_names.begin(), sensor_names.end(), "ref_locked") != sensor_names.end()) {    // if "ref_locked" sensor is detected
            uhd::sensor_value_t ref_locked = rx_usrp->get_mboard_sensor("ref_locked", 0);
            std::cout << boost::format("RX reference locking status: %s ...") % ref_locked.to_pp_string()
                    << std::endl;
            UHD_ASSERT_THROW(ref_locked.to_bool());
        }
        printfoot();

        

        // --- print overall USRP info ---
        // std::cout << boost::format("RX USRP Device Info: %s") % rx_usrp->get_pp_string() << std::endl;
        std::cout << "=========================" << std::endl; 
        std::cout << "Configuration Success!" << std::endl;
        std::cout << "=========================" << std::endl << std::endl;
        
        // --- return object ---
        return rx_usrp;
    }



//// ====== Define stop mark ======
    /*!
    * @brief Define the stop signal
    */
    std::atomic<bool> stop_signal_called{false};
    void sig_int_handler(int signum)
    {
        stop_signal_called = true;
    }



//// ====== Create Automatic coordiniates ======
    /*!
    * @brief Define the coordinates of the PAAM movement on the XY table, and 
    *       the coordinates of the PAAM steering directions. 
    */
    std::vector<int> make_grids(
                            int grids_init,
                            int grids_step,
                            int grids_end)
    {
        int num_grids = (grids_end - grids_init) / grids_step + 1;
        int mid_grids;
        std::vector<int> grids_ascend_sorted(num_grids);
        std::vector<int> grids(num_grids);
        

        for (int i = 0; i < num_grids; ++i) {
            grids_ascend_sorted[i] = grids_init + i * grids_step;
        }
        mid_grids = (num_grids + 1) / 2;
        for (int i = 0; i < mid_grids; ++i) {
            grids[2 * i] = grids_ascend_sorted[i];
            if(2 * i + 1 < num_grids){
                grids[2 * i + 1] = grids_ascend_sorted[i + mid_grids];
            }
        }

        return grids;
    }





//// ====== Create dated folder ======
    /*!
     * @brief Detect the system date and create a seperate folder for all the data that measured during the same day. 
     * 
     */
    std::string dated_folder()
    {
        // get the current time 
        std::time_t now = std::time(nullptr);

        // convert time to the system time
        std::tm* local_time = std::localtime(&now); 

        // create a stringstream object to hold the date string
        std::stringstream ss;

        // Write year, month, and day into the stringstream
        ss << 1900 + local_time->tm_year; // year that starts from 1900
        ss.fill('0'); // fill with 0s for single digit months and days
        ss.width(2);  // ensure two digits for month and day
        ss << 1 + local_time->tm_mon;    // month that ranges from 0 (Jan) to 11 (Dec)
        ss.width(2); 
        ss << local_time->tm_mday;       // day

        // create directory path
        struct stat info;
        size_t dated_folder_creation_result;
        std::string data_path = main_data_path + ss.str();

        // using 0777 to give read, write, execute permissions to all users
        if (stat(data_path.c_str(), &info) != 0) {
            dated_folder_creation_result = mkdir(data_path.c_str(), 0777);    
            // check if directory was created successfully
            if (dated_folder_creation_result != 0) {
                std::cout << "Error creating directory at: " << data_path << std::endl;
            } 
        }
        return data_path;
    }

    std::string dated_folder(std::string meas_name)
    {
        // get the current time 
        std::time_t now = std::time(nullptr);

        // convert time to the system time
        std::tm* local_time = std::localtime(&now); 

        // create a stringstream object to hold the date string
        std::stringstream ss;

        // Write year, month, and day into the stringstream
        ss << 1900 + local_time->tm_year; // year that starts from 1900
        ss.fill('0'); // fill with 0s for single digit months and days
        ss.width(2);  // ensure two digits for month and day
        ss << 1 + local_time->tm_mon;    // month that ranges from 0 (Jan) to 11 (Dec)
        ss.width(2); 
        ss << local_time->tm_mday;       // day

        // create directory path
        struct stat info;
        size_t dated_folder_creation_result_1;
        size_t dated_folder_creation_result_2;
        std::string data_path = main_data_path + ss.str() + "/" + meas_name;
        std::string parent_data_path = main_data_path + ss.str();
        // using 0777 to give read, write, execute permissions to all users
        if (stat(data_path.c_str(), &info) != 0) {
            dated_folder_creation_result_1 = mkdir(parent_data_path.c_str(), 0777);
            dated_folder_creation_result_2 = mkdir(data_path.c_str(), 0777);
            // check if directory was created successfully
            if (dated_folder_creation_result_1 || dated_folder_creation_result_2 != 0) {
                std::cout << "Error creating directory at: " << data_path << std::endl;
            }
        }
        return data_path;
    }
