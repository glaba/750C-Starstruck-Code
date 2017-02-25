/** @file autonrecorder.c
 * @brief File for autonomous recorder code
 *
 * This file contains the code for the saving, loading, and playback of autonomous files.
 * When an autonomous routine is recorded, it is saved to a file to flash memory.
 * This file is loaded and executed during the autonomous period of the game.
 * It works by saving the motor values at a point in time.
 * At the corresponding point in time, the values are played back.
 *
 * This file also handles the recording of programming skills by stitching 4 autonomous routines together.
 */

#include "main.h"
#include <string.h>
#include <stdlib.h>

/**
 * Stores the joystick state variables for moving the robot.
 * Used for recording and playing back autonomous routines.
 */
joyState states[AUTON_TIME*JOY_POLL_FREQ];

/**
 * Slot number of currently loaded autonomous routine.
 */
int autonLoaded;

/**
 * Whether or not the autonomous should be flipped (-1 if so, 1 if not)
 */
int autonFlipped = 1;

/**
 * Section number (0-3) of currently loaded programming skills routine.
 */
int progSkills;

/**
 * Initializes autonomous recorder by setting states array to zero.
 */
void initAutonRecorder() {
    printf("Beginning initialization of autonomous recorder...\n");
    lcdClear(LCD_PORT);
    lcdSetText(LCD_PORT, 1, "Init recorder...");
    lcdSetText(LCD_PORT, 2, "");
    memset(states, 0, sizeof(*states));
    printf("Completed initialization of autonomous recorder.\n");
    lcdSetText(LCD_PORT, 1, "Init-ed recorder!");
    lcdSetText(LCD_PORT, 2, "");
    autonLoaded = 0;
    progSkills = 0;
}

/**
 * Records driver joystick values into states array.
 */
void recordAuton() {
    lcdClear(LCD_PORT);
    for(int i = 3; i > 0; i--){
        lcdSetBacklight(LCD_PORT, true);
        printf("Beginning autonomous recording in %d...\n", i);
        lcdSetText(LCD_PORT, 1, "Recording auton");
        lcdPrint(LCD_PORT, 2, "in %d...", i);
        delay(1000);
    }
    printf("Ready to begin autonomous recording.\n");
    lcdSetText(LCD_PORT, 1, "Recording auton...");
    lcdSetText(LCD_PORT, 2, "");
    bool lightState = false;
    for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
        printf("Recording state %d...\n", i);
        lcdSetBacklight(LCD_PORT, lightState);
        lightState = !lightState;
        recordJoyInfo();
        states[i].spd = spd;
        states[i].horizontal = horizontal;
        states[i].turn = turn;
        states[i].sht = sht;
        states[i].lift = lift;
        printf("Record State %d, Speed: %d %d %d %d %d\n", i, states[i].spd, states[i].horizontal, states[i].turn, states[i].sht, states[i].lift);
        if (joystickGetDigital(1, 7, JOY_UP)) {
            printf("Autonomous recording manually cancelled.\n");
            lcdSetText(LCD_PORT, 1, "Cancelled record.");
            lcdSetText(LCD_PORT, 2, "");
            memset(states + i + 1, 0, sizeof(joyState) * (AUTON_TIME * JOY_POLL_FREQ - i - 1));
            i = AUTON_TIME * JOY_POLL_FREQ;
        }
        moveRobot();
        delay(1000 / JOY_POLL_FREQ);
    }
    lcdSetBacklight(LCD_PORT, true);

    printf("Completed autonomous recording.\n");
    lcdSetText(LCD_PORT, 1, "Recorded auton!");
    lcdSetText(LCD_PORT, 2, "");
    motorStopAll();
    delay(1000);
    autonLoaded = 0;

}

/**
 * Saves contents of the states array to a file in flash memory.
 */
void saveAuton() {
    printf("Waiting for file selection...\n");
    lcdClear(LCD_PORT);
    lcdSetText(LCD_PORT, 1, "Save to?");
    lcdSetText(LCD_PORT, 2, "");
    int autonSlot;
    if(progSkills == 0) {
        autonSlot = selectAuton(false);
    } else {
        printf("Currently in the middle of a programming skills run.\n");
        autonSlot = MAX_AUTON_SLOTS + 1;
    }
    if(autonSlot == 0) {
        printf("Not saving this autonomous!\n");
        delay(1000);
        return;
    }
    lcdSetText(LCD_PORT, 1, "Saving auton...");
    char filename[AUTON_FILENAME_MAX_LENGTH];
    if(autonSlot != MAX_AUTON_SLOTS + 1 && autonSlot > 0) {
        printf("Not doing programming skills, recording to slot %d.\n",autonSlot);
        snprintf(filename, sizeof(filename)/sizeof(char), "a%d", autonSlot);
        lcdPrint(LCD_PORT, 2, "Slot: %d", autonSlot);
    } else if (autonSlot < 0) {
        printf("Invalid autonomous selection.");
        delay(1000);
        return;
    } else {
        printf("Doing programming skills, recording to section %d.\n", progSkills);
        snprintf(filename, sizeof(filename)/sizeof(char), "p%d", progSkills);
        lcdPrint(LCD_PORT, 2, "Skills Part: %d", progSkills+1);
    }
    printf("Saving to file %s...\n",filename);
    FILE *autonFile = fopen(filename, "w");
    if (autonFile == NULL) {
        printf("Error saving autonomous in file %s!\n", filename);
        lcdSetText(LCD_PORT, 1, "Error saving!");
        if(autonSlot != MAX_AUTON_SLOTS + 1){
            printf("Not doing programming skills, error saving auton in slot %d!\n", autonSlot);
            lcdSetText(LCD_PORT, 1, "Error saving!");
            lcdPrint(LCD_PORT, 2,   "Slot: %d", autonSlot);
        } else {
            printf("Doing programming skills, error saving auton in section 0!\n");
            lcdSetText(LCD_PORT, 1, "Error saving!");
            lcdSetText(LCD_PORT, 2, "Prog. Skills");
        }
        delay(1000);
        return;
    }
    signed char write[5];
    for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
        printf("Recording state %d to file %s...\n", i, filename);
        write[0] = states[i].spd;
        write[1] = states[i].horizontal;
        write[2] = states[i].turn;
        write[3] = states[i].sht;
        write[4] = states[i].lift;

        printf("Save State %d, Speed: %d %d %d %d %d\n", i, write[0], write[1], write[2], write[3], write[4]);
        for (int j = 0; j < 5; j++) {
            fwrite(write + j, sizeof(char), sizeof(char), autonFile);
        }
        delay(10);
    }
    fclose(autonFile);
    printf("Completed saving autonomous to file %s.\n", filename);
    lcdSetText(LCD_PORT, 1, "Saved auton!");
    if(autonSlot != MAX_AUTON_SLOTS + 1) {
        printf("Not doing programming skills, recorded to slot %d.\n",autonSlot);
        lcdPrint(LCD_PORT, 2, "Slot: %d", autonSlot);
    } else {
        printf("Doing programming skills, recorded to section %d.\n", progSkills);
        lcdPrint(LCD_PORT, 2, "Skills Part: %d", progSkills+1);
    }
    delay(1000);
    if(autonSlot == MAX_AUTON_SLOTS + 1) {
        printf("Proceeding to next programming skills section (%d).\n", ++progSkills);
    }
    if(progSkills == PROGSKILL_TIME/AUTON_TIME) {
        printf("Finished recording programming skills (all parts).\n");
        progSkills = 0;
    }
    autonLoaded = autonSlot;
}

/**
 * Downloads a 15 second autonomous portion from the computer through the serial monitor
 *
 * @param slot A number from 1 - 10 for a regular autonomous routine, or a number from -1 to -4 for a programming skills slot
 */
void downloadAutonFromComputer(int slot) {
    motorStopAll();

    char filename[AUTON_FILENAME_MAX_LENGTH + 1];
    if (slot >= 1 && slot <= 10) {
        snprintf(filename, sizeof(filename), "a%d", slot);
    } else if (slot <= -1 && slot >= -4) {
        snprintf(filename, sizeof(filename), "p%d", -slot - 1);
    }

    FILE* autonFile = fopen(filename, "w");
    if (autonFile == NULL) {
        printf("Writing to autonomous file failed. \n");
        lcdSetText(LCD_PORT, 1, "Failed to open!");
        return;
    } else {
        printf("Please input the autonomous file fully in the serial input \n");
        lcdSetText(LCD_PORT, 1, "Waiting for input...");

        signed char read[5] = {0, 0, 0, 0, 0};
        for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
            lcdPrint(LCD_PORT, 2, "Pull state %d", i);
            for (int j = 0; j < 5; j++) {
                read[j] = getchar();
                if (read[j] == -1) {//(fread(read + j, sizeof(char), sizeof(char), stdin) == 0) {
                    printf("Not enough input for a full autonomous file, ending at state %d", i);
                    fclose(autonFile);
                    return;
                }
            }
            states[i].spd = (signed char) read[0];
            states[i].horizontal = (signed char) read[1];
            states[i].turn = (signed char) read[2];
            states[i].sht = (signed char) read[3];
            states[i].lift = (signed char) read[4];
            delay(20);
        }

        lcdPrint(LCD_PORT, 1, "Writing to file...");
        lcdSetText(LCD_PORT, 2, "");

        signed char write[5];
        for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
            printf("Recording state %d to file %s...\n", i, filename);
            write[0] = states[i].spd;
            write[1] = states[i].horizontal;
            write[2] = states[i].turn;
            write[3] = states[i].sht;
            write[4] = states[i].lift;

            printf("Save State %d, Speed: %d %d %d %d %d\n", i, write[0], write[1], write[2], write[3], write[4]);
            for (int j = 0; j < 5; j++) {
                fwrite(write + j, sizeof(char), sizeof(char), autonFile);
            }
            delay(10);
        }
    }
}

/**
 * Uploads a 15 second autonomous portion to the computer through the serial monitor
 *
 * @param slot A number from 1 - 10 for a regular autonomous routine, or a number from -1 to -4 for a programming skills slot
 */
void uploadAutonToComputer(int slot) {
    char filename[AUTON_FILENAME_MAX_LENGTH + 1];
    if (slot >= 1 && slot <= 10) {
        snprintf(filename, sizeof(filename), "a%d", slot);
    } else if (slot <= -1 && slot >= -4) {
        snprintf(filename, sizeof(filename), "p%d", -slot - 1);
    }

    FILE* autonFile = fopen(filename, "r");
    if (autonFile == NULL) {
        printf("Reading from autonomous file failed. \n");
        lcdSetText(LCD_PORT, 1, "Failed to open!");
        return;
    } else {
        lcdSetText(LCD_PORT, 1, "Uploading...");
        lcdSetText(LCD_PORT, 2, "");
        printf("Sending file...\n");
        printf("----------\n");
        char read[5] = {0, 0, 0, 0, 0};
        for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
            for (int j = 0; j < 5; j++) {
                fseek(autonFile, 5 * i + j, SEEK_SET);
                fread(read + j, sizeof(char), sizeof(char), autonFile);
            }
            printf("%c%c%c%c%c", read[0], read[1], read[2], read[3], read[4]);
            delay(20);
        }
        printf("----------\n");
        printf("File ended.\n");
        fclose(autonFile);
    }
}

/**
 * Gets the autonomous selection from the LCD buttons
 * 
 * @param allowProgSkillSection Allows the user to select a specific programming skills section slot if this is true
 *
 * @return the autonomous selected (slot number), or if allowProgSkillSection is true and a programming skills is selected, the negative of the programming skill section number from -1 to -4
 */
int selectAuton(int allowProgSkillSection) {
    printf("Waiting for file selection...\n");
    lcdSetText(LCD_PORT, 1, "Select file");
    lcdSetText(LCD_PORT, 2, "None");

    int curSlot = 0;

    int prevLCDLeft = 0;
    int prevLCDRight = 0;
    while ((lcdReadButtons(LCD_PORT) & LCD_BTN_CENTER) == 0) {
        if (((lcdReadButtons(LCD_PORT) & LCD_BTN_RIGHT) != 0) && prevLCDRight == 0) {
            curSlot = (curSlot + 1) % (MAX_AUTON_SLOTS + 2);
        } else if (((lcdReadButtons(LCD_PORT) & LCD_BTN_LEFT) != 0) && prevLCDLeft == 0) {
            curSlot--;
            if (curSlot == -1) {
                curSlot = MAX_AUTON_SLOTS + 1;
            }
        }

        if (curSlot == 0) {
            lcdSetText(LCD_PORT, 2, "None");
        } else if (curSlot == MAX_AUTON_SLOTS + 1) {
            lcdSetText(LCD_PORT, 2, "Programming skills");
        } else {
            char filename[AUTON_FILENAME_MAX_LENGTH];
            snprintf(filename, sizeof(filename)/sizeof(char), "a%d", curSlot);
            FILE* autonFile = fopen(filename, "r");

            if(autonFile == NULL){
                lcdPrint(LCD_PORT, 2, "Slot: %d (EMPTY)", curSlot);
            } else {
                lcdPrint(LCD_PORT, 2, "Slot: %d", curSlot);
                fclose(autonFile);
            }
        }

        prevLCDLeft = lcdReadButtons(LCD_PORT) & LCD_BTN_LEFT;
        prevLCDRight = lcdReadButtons(LCD_PORT) & LCD_BTN_RIGHT;

        delay(20);
    }
    delay(500);

    prevLCDLeft = 0;
    prevLCDRight = 0;
    int curProgSkillSection = 0;
    if ((curSlot == MAX_AUTON_SLOTS + 1) && allowProgSkillSection) {
        while ((lcdReadButtons(LCD_PORT) & LCD_BTN_CENTER) == 0) {
            lcdSetText(LCD_PORT, 1, "Prog. Skills Slot");
            
            if (((lcdReadButtons(LCD_PORT) & LCD_BTN_RIGHT) != 0) && prevLCDRight == 0) {
                curProgSkillSection = (curProgSkillSection + 1) % (PROGSKILL_TIME / AUTON_TIME);
            } else if (((lcdReadButtons(LCD_PORT) & LCD_BTN_LEFT) != 0) && prevLCDLeft == 0) {
                curProgSkillSection--;
                if (curProgSkillSection == -1) {
                    curProgSkillSection = 3;
                }
            }

            lcdPrint(LCD_PORT, 1, "Prog. Skills Part %d", curProgSkillSection + 1);

            delay(50);
        }
        return -curProgSkillSection - 1;
    }

    delay(500);
    return curSlot;
}

/**
 * Loads autonomous file contents into states array.
 *
 * @param autonSlot The slot of the autonomous to load. If this value is MAX_AUTON_SLOTS + 1, it will load the programming skills run instead.
 */
void loadAuton(int autonSlot) {
    lcdClear(LCD_PORT);
    FILE* autonFile;
    char filename[AUTON_FILENAME_MAX_LENGTH];

    if(autonSlot == 0) {
        printf("Not loading an autonomous!\n");
        lcdSetText(LCD_PORT, 1, "Not loading!");
        lcdSetText(LCD_PORT, 2, "");
        autonLoaded = 0;
        return;
    } else if(autonSlot == MAX_AUTON_SLOTS + 1){
        printf("Performing programming skills.\n");
        lcdSetText(LCD_PORT, 1, "Loading skills...");
        lcdPrint(LCD_PORT,   2, "Skills Part: 1");
        autonLoaded = MAX_AUTON_SLOTS + 1;
    } else if (autonSlot == MAX_AUTON_SLOTS + 2) {
        printf("Performing hard-coded programming skills.\n");
        lcdSetText(LCD_PORT, 1, "Loaded skills!");
        lcdPrint(LCD_PORT,   2, "Hardcoded Skills");
        autonLoaded = MAX_AUTON_SLOTS + 2;
        return;
    } else if(autonSlot == autonLoaded) {
        printf("Autonomous %d is already loaded.\n", autonSlot);
        lcdSetText(LCD_PORT, 1, "Loaded auton!");
        lcdPrint(LCD_PORT,   2, "Slot: %d", autonSlot);
        return;
    } else if (autonSlot < 0) {
        printf("Invalid autonomous selection");
        return;
    }
    printf("Loading autonomous from slot %d...\n", autonSlot);
    lcdSetText(LCD_PORT, 1, "Loading auton...");
    if(autonSlot != MAX_AUTON_SLOTS + 1){
        lcdPrint(LCD_PORT, 2,   "Slot: %d", autonSlot);
    }
    if(autonSlot != MAX_AUTON_SLOTS + 1){
        printf("Not doing programming skills, loading slot %d\n", autonSlot);
        snprintf(filename, sizeof(filename)/sizeof(char), "a%d", autonSlot);
    } else {
        printf("Doing programming skills, loading section 0.\n");
        snprintf(filename, sizeof(filename)/sizeof(char), "p0");
    }
    printf("Loading from file %s...\n",filename);
    autonFile = fopen(filename, "r");
    if (autonFile == NULL) {
        printf("No autonomous was saved in file %s!\n", filename);
        lcdSetText(LCD_PORT, 1, "No auton saved!");
        if(autonSlot != MAX_AUTON_SLOTS + 1){
            printf("Not doing programming skills, no auton in slot %d!\n", autonSlot);
            lcdSetText(LCD_PORT, 1, "No auton saved!");
            lcdPrint(LCD_PORT, 2,   "Slot: %d", autonSlot);
        } else {
            printf("Doing programming skills, no auton in section 0!\n");
            lcdSetText(LCD_PORT, 1, "No skills saved!");
        }
        return;
    }

    fseek(autonFile, 0, SEEK_SET);
    signed char read[5];
    for (int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
        printf("Loading state %d from file %s...\n", i, filename);
        for (int j = 0; j < 5; j++) {
            fseek(autonFile, 5 * i + j, SEEK_SET);
            fread(read + j, sizeof(char), sizeof(char), autonFile);
        }
        states[i].spd = (signed char) read[0];
        states[i].horizontal = (signed char) read[1];
        states[i].turn = (signed char) read[2];
        states[i].sht = (signed char) read[3];
        states[i].lift = (signed char) read[4];
        printf("Load State %d, Speed: %d %d %d %d %d\n", i, states[i].spd, states[i].horizontal, states[i].turn, states[i].sht, states[i].lift);
        delay(10);
    }
    fclose(autonFile);
    printf("Completed loading autonomous from file %s.\n", filename);
    lcdSetText(LCD_PORT, 1, "Loaded auton!");
    if(autonSlot != MAX_AUTON_SLOTS + 1){
        printf("Not doing programming skills, loaded from slot %d.\n", autonSlot);
        lcdPrint(LCD_PORT, 2, "Slot: %d", autonSlot);
    } else {
        printf("Doing programming skills, loaded from section %d.\n", progSkills);
        lcdSetText(LCD_PORT, 2, "Skills Section: 1");
    }
    autonLoaded = autonSlot;
}

/**
 * Replays autonomous based on loaded values in states array.
 *
 * @param flipped -1 if the autonomous should be flipped over the y axis (for the opposite starting tile), 1 otherwise
 */
void playbackAuton(int flipped) { //must load autonomous first!
    if(autonLoaded == 0) {
        printf("autonLoaded = 0, doing nothing.\n");
        return;
    }
    printf("Beginning playback...\n");
    lcdSetText(LCD_PORT, 1, "Playing back...");
    lcdSetText(LCD_PORT, 2, "");
    lcdSetBacklight(LCD_PORT, true);
    int file = 0;
    do{
        FILE* nextFile = NULL;
        lcdPrint(LCD_PORT, 2, "File: %d", file+1);
        char filename[AUTON_FILENAME_MAX_LENGTH];
        if(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME - 1){
            printf("Next section: %d\n", file+1);
            snprintf(filename, sizeof(filename)/sizeof(char), "p%d", file+1);
            nextFile = fopen(filename, "r");
        }
        for(int i = 0; i < AUTON_TIME * JOY_POLL_FREQ; i++) {
            spd = states[i].spd;
            horizontal = states[i].horizontal;
            turn = states[i].turn;
            sht = states[i].sht;
            lift = states[i].lift;
            printf("Playback State: %d, Speed: %d %d %d %d %d\n", i, states[i].spd, states[i].horizontal, states[i].turn, states[i].sht, states[i].lift);
            if (joystickGetDigital(1, 7, JOY_UP) && !isOnline()) {
                printf("Playback manually cancelled.\n");
                lcdSetText(LCD_PORT, 1, "Cancelled playback.");
                lcdSetText(LCD_PORT, 2, "");
                i = AUTON_TIME * JOY_POLL_FREQ;
                file = PROGSKILL_TIME/AUTON_TIME;
            }
            moveRobot();
            if(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME - 1){
                printf("Loading state %d from file %s...\n", i, filename);

                char read[5] = {0, 0, 0, 0, 0};
                for (int j = 0; j < 5; j++) {
                    fseek(nextFile, 5 * i + j, SEEK_SET);
                    fread(read + j, sizeof(char), sizeof(char), nextFile);
                }
                states[i].spd = (signed char) read[0];
                states[i].horizontal = (signed char) read[1];
                states[i].turn = (signed char) read[2];
                states[i].sht = (signed char) read[3];
                states[i].lift = (signed char) read[4];
                printf("Load State %d, Speed: %d %d %d %d %d\n", i, states[i].spd, states[i].horizontal, states[i].turn, states[i].sht, states[i].lift);
            }
            delay(1000 / JOY_POLL_FREQ);
        }
        if(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME - 1){
            printf("Finished with section %d, closing file.\n", file+1);
            fclose(nextFile);
        }
        file++;
    } while(autonLoaded == MAX_AUTON_SLOTS + 1 && file < PROGSKILL_TIME/AUTON_TIME);
    motorStopAll();
    printf("Completed playback.\n");
    lcdSetText(LCD_PORT, 1, "Played back!");
    lcdSetText(LCD_PORT, 2, "");
    delay(1000);
}
