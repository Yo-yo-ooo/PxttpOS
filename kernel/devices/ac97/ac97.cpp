#include "ac97.h"
#include "../../osData/osData.h"
#include "../../rendering/testoDebug.h"
#include "../serial/serial.h"
#include "../pit/pit.h"
#include "../../interrupts/panic.h"
#include <libm/cstr.h>
#include <libm/memStuff.h>
#include "../../paging/PageTableManager.h"
#include "../../audio/audioDevStuff.h"


// https://github.com/byteduck/duckOS/blob/master/kernel/device/AC97Device.cpp

namespace AC97
{
    void AC97Driver::HandleIRQ(interrupt_frame* frame)
    {
        lastCheckTime = PIT::TimeSinceBootMS();
        //Panic("OO YES", true);
        //Serial::Writeln("<AC97 IRQ>");
        // if (!handle_irq())
        // {
        //     //Serial::Writeln("</NVM>");
        //     //return;
        // }
        
        
        handle_irq();
        doCheck = true;
        //Serial::Writeln("</AC97 IRQ>");
        
        

        //needManualRestart = CheckMusic();    
         

        // if (osData.ac97Driver != NULL)
        // {
        //     //AddToStack();
        //     int amt = 1000;
        //     int step = 1;
        //     int hz = ((PIT::TimeSinceBootMS() / 10) % 100) * 2 + 200;
        //     //hz = 200;
        //     // frequency gets doubled bc we are sending mono
        //     uint16_t* testBuff = (uint16_t*)_Malloc(amt*2);

        //     MusicBit16Test::FillArray(testBuff, 0, amt, hz);            
        
        //     //AddToStack();
        //     uint64_t tCount = 0;
        //     tCount = osData.ac97Driver->writeBuffer(0, (uint8_t*)testBuff, amt*2);
        //     //RemoveFromStack();
        //     //Serial::Writeln("X> Wrote {} bytes", to_string(tCount));


        //     //AddToStack();
        //     _Free(testBuff);
        //     //RemoveFromStack();
        // }

        //Serial::Writeln("</AC97 IRQ: {}>", to_string(needManualRestart));  
    }

    bool AC97Driver::DoQuickCheck()
    {
        if (QuickCheck)
        {
            return false;
        }


        QuickCheck = true;


        if (samplesReady < 1)
        {
            audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
            samplesReady = audioDestination->RequestBuffers();
            AudioDeviceStuff::reqMoreData(audioDestination);
        }

        if (samplesReady > 0 && 
            osData.ac97Driver != NULL)
        {
            
            uint64_t tCount = 0;
            //Serial::Writeln("> Writing {} bytes", to_string(audioDestination->buffer->byteCount));
            // tCount = osData.ac97Driver->writeBuffer(0, 
            //     (uint8_t*)(audioDestination->buffer->data), 
            //     audioDestination->buffer->byteCount);

            //samplesReady = audioDestination->buffer->totalSampleCount;


            int byteCount = samplesReady * (audioDestination->buffer->bitsPerSample / 8) * audioDestination->buffer->channelCount; // 16 bit, stereo
            tCount = osData.ac97Driver->writeBuffer(0, 
                (uint8_t*)(audioDestination->buffer->data), 
                byteCount);
            //Serial::TWritelnf("> Writing %d/%d bytes (%d/%d samples) -> %d", byteCount, audioDestination->buffer->byteCount, samplesReady,audioDestination->buffer->totalSampleCount, tCount);

            if (tCount != byteCount)
            {
                Panic("AC97Driver::HandleIRQ: tCount != byteCount", true);
            }

            audioDestination->buffer->ClearBuffer();
            audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
            //lastDone = tDone;
            //Panic("bruh: {}", to_string(c), true);
            //Serial::Write("NICE");

            //Serial::Writeln("<WROTE LE MUSIC>");
            QuickCheck = false;
            samplesReady = 0;
            return true;
        }
        else
        {
            // TODO: SEND MSG TO REQ AUDIO
            //AudioDeviceStuff::reqMoreData(audioDestination);
        }
        
        

        // if (!dataReady)
        // {
        //     int c = audioDestination->RequestBuffers();
        //     if (c > 0)
        //     {
        //         dataReady = true;
        //     }
        // }


        QuickCheck = false;
        return false;
    }

    bool AC97Driver::CheckMusic()
    {
        //Serial::Writeln("<AC97 CheckMusic>");
        if (samplesReady > 0)
        {
            return false;
            bool ret =!handle_irq(); 
            //Serial::Writeln("</AC97 CheckMusic: {}>", to_string(ret));
            return ret;
        }
        //return true;


        audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
        samplesReady = audioDestination->RequestBuffers();
        AudioDeviceStuff::reqMoreData(audioDestination);
        if (samplesReady > 0)
        {
            //Serial::Writeln("</AC97 CheckMusic: {}>", to_string(false));
            return false;
        }
        else
        {
            // TODO: SEND MSG TO REQ AUDIO
        }
        

        
        //Serial::Writeln("</AC97 CheckMusic: {}>", to_string(true));
        return true;

        // if (audioDestination != NULL)
        // {
        //     bool tDone = audioDestination->AllBuffersDone();
        //     if (tDone)
        //     {
        //         if (!lastDone && 
        //             osData.ac97Driver != NULL)
        //         {
        //             // Do Stuff
        //             uint64_t tCount = 0;
        //             tCount = osData.ac97Driver->writeBuffer(0, 
        //             (uint8_t*)(audioDestination->buffer->data), 
        //             audioDestination->buffer->byteCount);

        //             if (tCount != audioDestination->buffer->byteCount)
        //             {
        //                 Panic("AC97Driver::HandleIRQ: tCount != audioDestination->buffer->byteCount", true);
        //             }

        //             audioDestination->buffer->ClearBuffer();
        //             audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
        //             lastDone = tDone;
        //             return false;
        //         }
        //     }
        //     else
        //     {
        //         audioDestination->RequestBuffers();
        //     }
        //     lastDone = tDone;
        //     return true;
        // }
    }

    AC97Driver::AC97Driver(PCI::PCIDeviceHeader* pciBaseAddress)
    {
        PCIBaseAddress = pciBaseAddress;
        uint64_t address = (uint64_t)PCIBaseAddress;
        PrintMsgStartLayer("AC97Driver");

        PrintMsg("> AC97 Driver Init");

        // PrintMsg("> Disabled PCI IO SPACE");
        // PCI::disable_io_space((uint64_t)PCIBaseAddress);
        // io_wait(5000);
        // PrintMsg("> Disabled PCI MEM SPACE");
        // PCI::disable_mem_space((uint64_t)PCIBaseAddress);
        // io_wait(5000);

        PrintMsg("> Enabled PCI IO SPACE");
        PCI::enable_io_space(address);
        io_wait(500);
        PrintMsg("> Enabled PCI MEM SPACE");
        PCI::enable_mem_space(address);
        io_wait(500);

        //Enable bus mastering and interrupts
        io_wait(500);
        PCI::enable_interrupt(address);
        PrintMsg("> Enabled PCI Interrupts");
        io_wait(500);
        PCI::enable_bus_mastering(address);
        PrintMsg("> Enabled PCI Bus Mastering");
        Println();
        

        
        PrintfMsgCol("> AC97 REV: %d", Colors.bgreen, ((PCI::PCIDeviceHeader*)PCIBaseAddress)->Revision_ID);

        for (int i = 0; i < 6; i++)
        {
            PrintfMsgCol("> BAR %d: %X", Colors.orange, i, (*(((uint32_t*)&((PCI::PCIHeader0*)PCIBaseAddress)->BAR0) + i)));
            io_wait(1000);
        }


        bool driverOkay = true;
        if (((PCI::PCIHeader0*)PCIBaseAddress)->BAR1 == 0)
        {
            PrintMsg("> AC97 Driver only uses BAR0 and is not supported");
            PrintMsgEndLayer("AC97Driver");
            return;
        }        

        // m_address = address;
        //m_mixer_address = PCI::io_read_word(address, PCI_BAR0) & ~1;
        m_mixer_type = PCI::pci_get_bar((PCI::PCIHeader0*)address, 0);

        //PrintMsgCol("> Mixer Address: {}", ConvertHexToString(m_mixer_address), Colors.yellow);
        //PrintMsgCol("> Mixer Address (2): {}", ConvertHexToString(((PCI::PCIHeader0*)address)->BAR0), Colors.yellow);
        if (m_mixer_type.type == PCI::PCI_BAR_TYPE_ENUM::MMIO64)
        {
            PrintMsgCol("> BUS TYPE USING BAR2", Colors.orange);
            m_bus_type = PCI::pci_get_bar((PCI::PCIHeader0*)address, 2);
        }
        else
        {
            PrintMsgCol("> BUS TYPE USING BAR1", Colors.orange);
            m_bus_type = PCI::pci_get_bar((PCI::PCIHeader0*)address, 1);
        }

        // Soft Reset
        PrintfMsgCol("> AC97 SOFT RESET", Colors.orange);
        PCI::write_byte(address, m_bus_type, BusRegisters::GLOBAL_CONTROL, GlobalControl::COLD_RESET);
        io_wait(1000);
        PCI::write_byte(address, m_bus_type, BusRegisters::GLOBAL_CONTROL, 0);
        io_wait(1000);

        if (osData.ac97Driver != NULL)
        {
            PrintMsg("> AC97 Driver already exists");
            PrintMsgEndLayer("AC97Driver");
            return;
        }


        //PrintMsgCol("> PCI Address: {}", ConvertHexToString(address), Colors.yellow);
        //Println();
        //IRQHandler(PCI::read_byte(address, PCI_INTERRUPT_LINE)),
        irqId = PCI::io_read_byte(address, PCI_INTERRUPT_LINE);
        PrintfMsg("> AC97 IRQ: %d", irqId);
        {
            IRQHandlerCallbackFuncs[irqId] = (void*)&AC97Driver::HandleIRQ;
            IRQHandlerCallbackHelpers[irqId] = (void*)this;
        }




        //PrintMsgCol("> Mixer Address (3): {}", ConvertHexToString(m_mixer_address), Colors.yellow);
        
        //PrintMsgCol("> Bus Address: {}", ConvertHexToString(m_bus_address), Colors.yellow);
        //PrintMsgCol("> Bus Address (2): {}", ConvertHexToString(((PCI::PCIHeader0*)address)->BAR1), Colors.yellow);
        m_output_channel = /*m_bus_address +*/ BusRegisters::NABM_PCM_OUT;
        PrintfMsgCol("> Output Channel: %X", Colors.yellow, m_output_channel);
        Println();

        PrintMsgStartLayer("AC97 Info");

        PrintMsgCol("> TYPES: NONE, M64, M32, IO ", Colors.bgreen);

        PrintfMsgCol("> AC97 MIXER TYPE: %d", Colors.bgreen, m_mixer_type.type);
        PrintfMsgCol("> AC97 MIXER IO BASE: %X", Colors.bgreen, m_mixer_type.io_address);
        PrintfMsgCol("> AC97 MIXER MEM BASE: %X", Colors.bgreen, m_mixer_type.mem_address);

        PrintfMsgCol("> AC97 BUS TYPE: %d", Colors.bgreen, m_bus_type.type);
        PrintfMsgCol("> AC97 BUS IO BASE: %X", Colors.bgreen, m_bus_type.io_address);
        PrintfMsgCol("> AC97 BUS MEM BASE: %X", Colors.bgreen, m_bus_type.mem_address);

        PrintMsgEndLayer("AC97 Info");

        //m_output_buffer_region = MM.alloc_dma_region(0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        //m_output_buffer_descriptor_region = MM.alloc_dma_region(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);

        //m_output_buffer_region = (uint8_t*)_Malloc(0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        //m_output_buffer_descriptor_region = (uint8_t*)_Malloc(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);

        m_output_buffer_region = (uint8_t*)GlobalAllocator->RequestPages(AC97_OUTPUT_BUFFER_PAGES);
        GlobalPageTableManager.MapMemories((void*)m_output_buffer_region, (void*)m_output_buffer_region, AC97_OUTPUT_BUFFER_PAGES);
        _memset(m_output_buffer_region, 0, 0x1000 * AC97_OUTPUT_BUFFER_PAGES);
        PrintfMsgCol("> Output Buffer Region: %X",  Colors.yellow, (uint64_t)m_output_buffer_region);
        //Panic("BRUH {}", ConvertHexToString((uint64_t)m_output_buffer_region), true);
        m_output_buffer_descriptor_region = (uint8_t*)GlobalAllocator->RequestPages(1);//(uint8_t*)_Malloc(sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);
        GlobalPageTableManager.MapMemories((void*)m_output_buffer_descriptor_region, (void*)m_output_buffer_descriptor_region, 1);
        _memset(m_output_buffer_descriptor_region, 0, sizeof(BufferDescriptor) * AC97_NUM_BUFFER_DESCRIPTORS);
        PrintfMsgCol("> Output Buffer Descriptor Region: %X",  Colors.yellow, (uint64_t)m_output_buffer_descriptor_region);
        Println();
        writeBufferCount = 0;

        m_output_buffer_descriptors = (BufferDescriptor*) m_output_buffer_descriptor_region;

        

        //Initialize the card with cold reset of bus and mixer, enable interrupts
        int control = PCI::read_dword(address, m_bus_type, BusRegisters::GLOBAL_CONTROL);//inl(m_bus_address + BusRegisters::GLOBAL_CONTROL);
        control |= GlobalControl::COLD_RESET | GlobalControl::INTERRUPT_ENABLE;
        control &= ~(0b11 << 22);
        control |=   (0b00 << 22);
        PCI::write_dword(address, m_bus_type, BusRegisters::GLOBAL_CONTROL, control);//outl(m_bus_address + BusRegisters::GLOBAL_CONTROL, control);
        write_mixer(RESET, 1);
        PrintMsg("> Initialized Card");
        Println();

        //TODO: Verify version?

        //Set master volume and reset pcm out channel
        write_mixer(MASTER_VOLUME, 0);
        PrintMsg("> Set Master Volume");
        write_mixer(PCM_VOLUME, 0x0808);
        PrintMsg("> Set PCM Volume");
        reset_output();
        


        const int wantedSampleRate = 48000;

        PrintfMsg("> Setting Sample Rate to %d", wantedSampleRate);
        int tries = 10;
        while (tries-- > 0)
        {
            set_sample_rate(wantedSampleRate);
            if (m_sample_rate == wantedSampleRate)
                break;
            PIT::Sleep(10);
        }
        PrintfMsg("> Card Sample Rate: %d", (int)m_sample_rate);

        if (m_sample_rate != wantedSampleRate)
            PrintMsg("> Sample rate is borked!");//Panic("AC97: Failed to set sample rate! GOT: {}", to_string((int)m_sample_rate), true);
        
        reset_output();
        PrintMsg("> Reset Output");
        samplesReady = 0;
        QuickCheck = false;
        lastCheckTime = PIT::TimeSinceBootMS();

        DEF_SAMPLE_COUNT = m_sample_rate / 20;
        
        audioDestination = new Audio::BasicAudioDestination(
            //Audio::AudioBuffer::Create16Bit48KHzStereoBuffer(DEF_SAMPLE_COUNT)
            Audio::AudioBuffer::Create16Bit48KHzStereoBuffer(DEF_SAMPLE_COUNT)
        );
        audioDestination->buffer->sampleCount = audioDestination->buffer->totalSampleCount;
        //lastDone = true;
        needManualRestart = true;
        doCheck = true;

        if (osData.ac97Driver == NULL)
        {
            osData.ac97Driver = this;
            Audio::AudioOutputDevice* dev = new Audio::AudioOutputDevice("AC97 Device", audioDestination);
            osData.audioOutputDevices.Add(dev);
            osData.defaultAudioOutputDevice = dev;
            PrintMsg("> Set as main AC97 Driver");
        }

        PrintMsgEndLayer("AC97Driver");
    }


    bool AC97Driver::handle_irq() 
    {
        lastCheckTime = PIT::TimeSinceBootMS();
        //Read the status
        auto status_byte = PCI::read_word((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::STATUS);//inw(m_output_channel + ChannelRegisters::STATUS);
        BufferStatus status = {.value = status_byte};

        if(status.fifo_error)
            Serial::TWriteln("> AC97 GOT A FIFO ERROR!");//Panic("AC97 GOT FIFO ERROR!");//KLog::err("AC97", "Encountered FIFO error!");



        //osData.debugTerminalWindow->newPosition.x--;

        status.value = 0;
        status.completion_interrupt_status = true;
        status.last_valid_interrupt = true;
        status.fifo_error = true;
        PCI::write_word((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::STATUS, status.value);;//outw(m_output_channel + ChannelRegisters::STATUS, status.value);

        // DoQuickCheck();
        // return true;



        auto current_index = PCI::read_byte((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::CURRENT_INDEX);//inb(m_output_channel + ChannelRegisters::CURRENT_INDEX);
        auto last_valid_index = PCI::read_byte((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::LAST_VALID_INDEX);//inb(m_output_channel + ChannelRegisters::LAST_VALID_INDEX);
        
        int offset = 4;
        int beginIndex = (last_valid_index + AC97_NUM_BUFFER_DESCRIPTORS - offset) % AC97_NUM_BUFFER_DESCRIPTORS;
        int endIndex = (last_valid_index + 2) % AC97_NUM_BUFFER_DESCRIPTORS;

        if (endIndex >= beginIndex)
        {
            if (current_index >= beginIndex && current_index <= endIndex)
            {
                return DoQuickCheck();
            }
        }
        else
        {
            if (current_index >= beginIndex || current_index <= endIndex)
            {
                return DoQuickCheck();
            }
        }

        //If we're not done, don't do anything
        if(!status.completion_interrupt_status)
            return false;
        
        // if(current_index >= ((last_valid_index + AC97_NUM_BUFFER_DESCRIPTORS - 4) % AC97_NUM_BUFFER_DESCRIPTORS)) {
        //     //reset_output();
        //     DoQuickCheck();
        // }
        m_blocker = true;

        return false;
    }

    void AC97Driver::reset_output() 
    {
       // io_wait(1000);
        //Send reset command and wait for it to be cleared
        int timeOut = 200;
        do
        {
            PCI::write_byte((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::CONTROL, ControlFlags::RESET_REGISTERS);//outb(m_output_channel + ChannelRegisters::CONTROL, ControlFlags::RESET_REGISTERS);
            io_wait(10);
        } while((timeOut-- > 0) && PCI::read_byte((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::CONTROL) & ControlFlags::RESET_REGISTERS); //inb(m_output_channel + ChannelRegisters::CONTROL)

        m_output_dma_enabled = false;
        m_current_buffer_descriptor = 0;
        writeBufferCount = 0;
    }

    void AC97Driver::set_sample_rate(uint32_t sample_rate) {
        io_wait(1000);
        PCI::write_word((uint64_t)PCIBaseAddress, m_mixer_type, MixerRegisters::SAMPLE_RATE, sample_rate);//outw(m_mixer_address + MixerRegisters::SAMPLE_RATE, sample_rate);
        io_wait(1000);
        m_sample_rate = PCI::read_word((uint64_t)PCIBaseAddress, m_mixer_type, MixerRegisters::SAMPLE_RATE);//inw(m_mixer_address + MixerRegisters::SAMPLE_RATE);
    }


    uint64_t AC97Driver::writeBuffer(uint64_t offset, uint8_t* buffer, uint64_t count)
    {
        buffer += offset;
        count -= offset;
        //handle_irq();
        //Write buffer by buffer
        uint64_t n_written = 0;
        while(count > 0) 
        {
            // //Wait until we have a free buffer to write to
            // do 
            // {
            //     //Read the status, current index, and last valid index
            //     //TaskManager::ScopedCritical critical;
            //     auto status_byte = inw(m_output_channel + ChannelRegisters::STATUS);
            //     BufferStatus status = {.value = status_byte};
            //     auto current_index = inb(m_output_channel + ChannelRegisters::CURRENT_INDEX);
            //     auto last_valid_index = inb(m_output_channel + ChannelRegisters::LAST_VALID_INDEX);
            //     auto num_buffers_left = last_valid_index >= current_index ? last_valid_index - current_index : AC97_NUM_BUFFER_DESCRIPTORS - (current_index - last_valid_index);
            //     if(!status.is_halted)
            //         num_buffers_left++;
            //     if(num_buffers_left < AC97_NUM_BUFFER_DESCRIPTORS)
            //         break;
            //     //critical.exit();
            //     m_blocker = false;
            //     //TaskManager::current_thread()->block(m_blocker);
            //     // if(m_blocker.was_interrupted())
            //     //     return 0;
            // } while(m_output_dma_enabled);

            //If the output DMA is not currently enabled, reset the PCM channel to be sure
            if(!m_output_dma_enabled)
                reset_output();

            //reset_output();

            //Copy as much data as is applicable to the current output buffer
            uint32_t* output_buffer = (uint32_t*)(m_output_buffer_region + 0x1000 * m_current_output_buffer_page);
            size_t num_bytes = min(count, 0x1000);
            //buffer.read((uint8_t*) output_buffer, n_written, num_bytes); // buff offset count
            _memcpy((uint8_t*) buffer + n_written, output_buffer, num_bytes);
            count -= num_bytes;
            n_written += num_bytes;

            //Create the buffer descriptor
            auto* descriptor = &m_output_buffer_descriptors[m_current_buffer_descriptor];
            descriptor->data_addr = (uint32_t)(((uint64_t)m_output_buffer_region) + 0x1000 * m_current_output_buffer_page);//m_output_buffer_region->object()->physical_page(m_current_output_buffer_page).paddr();
            descriptor->num_samples = num_bytes / sizeof(uint16_t);
            descriptor->flags = {false, true};

            //Set the buffer descriptor list address and last valid index in the channel registers
            PCI::write_dword((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::BUFFER_LIST_ADDR, (uint32_t)(uint64_t)m_output_buffer_descriptor_region);//outl(m_output_channel + ChannelRegisters::BUFFER_LIST_ADDR, (uint32_t)(uint64_t)m_output_buffer_descriptor_region);//m_output_buffer_descriptor_region->object()->physical_page(0).paddr());
            PCI::write_byte((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::LAST_VALID_INDEX, m_current_buffer_descriptor);//outb(m_output_channel + ChannelRegisters::LAST_VALID_INDEX, m_current_buffer_descriptor);

            //If the output DMA is not enabled already, enable it
            if(!m_output_dma_enabled) {
                auto ctrl = PCI::read_byte((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::CONTROL);//inb(m_output_channel + ChannelRegisters::CONTROL);
                ctrl |= ControlFlags::PAUSE_BUS_MASTER | ControlFlags::ERROR_INTERRUPT | ControlFlags::COMPLETION_INTERRUPT;
                PCI::write_byte((uint64_t)PCIBaseAddress, m_bus_type, m_output_channel + ChannelRegisters::CONTROL, ctrl);//outb(m_output_channel + ChannelRegisters::CONTROL, ctrl);
                m_output_dma_enabled = true;
            }

            //Increment buffer page and buffer descriptor index
            m_current_output_buffer_page++;
            m_current_output_buffer_page %= AC97_OUTPUT_BUFFER_PAGES;
            m_current_buffer_descriptor++;
            m_current_buffer_descriptor %= AC97_NUM_BUFFER_DESCRIPTORS;
        }

        return n_written;
    }

}