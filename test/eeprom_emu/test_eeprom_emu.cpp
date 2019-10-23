#ifdef UNIT_TEST

#include <unity.h>

#include "eeprom_emu.h"
#include "eeprom_flash_driver.h"

/*
#include <stdio.h>

void mem_dump(EepromEmu<EepromFlashDriver> &eeprom)
{
    for (int i = 0; i < eeprom.flash.BankSize; i++)
    {
        printf("0x%.2X ", eeprom.flash.memory[i]);
    }

    printf("\n\n");

    for (int i = eeprom.flash.BankSize; i < eeprom.flash.BankSize*2; i++)
    {
        printf("0x%.2X ", eeprom.flash.memory[i]);
    }

    printf("\n\n");
}
*/

void test_eeprom_write() {
    EepromEmu<EepromFlashDriver> eeprom;

    eeprom.write_u32(3, 0x0000AA99);
    // bank Marker + address + value
    uint8_t expected[] = {
        0xFF, 0x7F, 0xFF, 0xFF,
        0x03, 0x20,
        0x99, 0xAA, 0x00, 0x00
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, eeprom.flash.memory, sizeof(expected));
}

void test_eeprom_write_over() {
    EepromEmu<EepromFlashDriver> eeprom;

    eeprom.write_u32(3, 0x0000AA99);
    eeprom.write_u32(3, 0x5577CCEE);

    uint8_t expected[] = {
        0xFF, 0x7F, 0xFF, 0xFF,
        0x00, 0x00,
        0x99, 0xAA, 0x00, 0x00,
        0x03, 0x20,
        0xEE, 0xCC, 0x77, 0x55
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, eeprom.flash.memory, sizeof(expected));
}

void test_eeprom_write_skip_the_same() {
    EepromEmu<EepromFlashDriver> eeprom;

    eeprom.write_u32(3, 0x0000AA99);
    // bank Marker + address + value
    uint8_t expected[] = {
        0xFF, 0x7F, 0xFF, 0xFF,
        0x03, 0x20,
        0x99, 0xAA, 0x00, 0x00
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, eeprom.flash.memory, sizeof(expected));
    eeprom.write_u32(3, 0x0000AA99);
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, eeprom.flash.memory, sizeof(expected));
}

void test_eeprom_read() {
    EepromEmu<EepromFlashDriver> eeprom;

    eeprom.write_u32(3, 0x0000AA99);
    TEST_ASSERT_EQUAL_HEX32(0x0000AA99, eeprom.read_u32(3, 0));
    eeprom.write_u32(3, 0x5577CCEE);
    TEST_ASSERT_EQUAL_HEX32(0x5577CCEE, eeprom.read_u32(3, 0));
}

void test_eeprom_bank_move() {
    EepromEmu<EepromFlashDriver> eeprom;

    // Fill bank to cause data move to next one
    uint16_t capacity = (eeprom.flash.BankSize - 4) / 6;
    uint32_t data = 0x7777CCCC;
    for (int i = 0; i < capacity; i++)
    {
        eeprom.write_u32(3, data);
        data ^= 0xFFFFFFFF;
    }
    // Overflow
    eeprom.write_u32(3, 0x0000AA99);
    // bank Marker + address + value
    uint8_t expected[] = {
        0xFF, 0x7F, 0xFF, 0xFF,
        0x03, 0x20,
        0x99, 0xAA, 0x00, 0x00
    };
    TEST_ASSERT_EQUAL_HEX8_ARRAY(expected, eeprom.flash.memory + eeprom.flash.BankSize, sizeof(expected));
}

void test_eeprom_float() {
    EepromEmu<EepromFlashDriver> eeprom;

    eeprom.write_float(3, 0.15F);
    TEST_ASSERT_EQUAL_HEX32(0.15F, eeprom.read_float(3, 0.0F));
    eeprom.write_float(3, 0.44F);
    TEST_ASSERT_EQUAL_HEX32(0.44F, eeprom.read_float(3, 0.0F));
}


int main() {
    UNITY_BEGIN();
    RUN_TEST(test_eeprom_write);
    RUN_TEST(test_eeprom_write_over);
    RUN_TEST(test_eeprom_write_skip_the_same);
    RUN_TEST(test_eeprom_read);
    RUN_TEST(test_eeprom_bank_move);
    RUN_TEST(test_eeprom_float);
    return UNITY_END();
}

#endif
