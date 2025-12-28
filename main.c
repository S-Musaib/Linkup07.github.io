#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 50
#define MAX_PHONE 20
#define FILENAME "contacts.dat"

// Structure to store contact information
typedef struct {
    int id;
    char name[MAX_NAME];
    char phone[MAX_PHONE];
} Contact;

// Function prototypes
void clearScreen();
void displayHeader();
void displayMenu();
void addContact();
void viewAllContacts();
void searchContact();
void editContact();
void deleteContact();
int getNextId();
void clearInputBuffer();
void pressEnterToContinue();
void toLowerCase(char *str);

int main() {
    int choice;
    
    while(1) {
        clearScreen(); 
        displayHeader();
        displayMenu();
        
        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1) {
            clearInputBuffer();
            printf("\n[ERROR] Invalid input! Please enter a number.\n");
            pressEnterToContinue();
            continue;
        }
        clearInputBuffer();
        
        switch(choice) {
            case 1: addContact(); break;
            case 2: viewAllContacts(); break;
            case 3: searchContact(); break;
            case 4: editContact(); break;
            case 5: deleteContact(); break;
            case 6:
                printf("\n*** Thank you for using LinkUp Contact Manager! ***\n");
                printf("*** Goodbye! ***\n\n");
                exit(0);
            default:
                printf("\n[ERROR] Invalid choice! Please select 1-6.\n");
                pressEnterToContinue();
        }
    }
    return 0;
}

// Optimized screen clearing for Visual Studio/Windows and Linux
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}



void displayHeader() {
    printf("\n================================================================\n");
    printf("                    LINKUP - CONTACT MANAGER                    \n");
    printf("         Lords Institute of Engineering and Technology         \n");
    printf("================================================================\n");
}

void displayMenu() {
    printf("----------------------------------------------------------------\n");
    printf("                          MAIN MENU                             \n");
    printf("----------------------------------------------------------------\n");
    printf("  1. Add New Contact\n");
    printf("  2. View All Contacts\n");
    printf("  3. Search Contact\n");
    printf("  4. Edit Contact\n");
    printf("  5. Delete Contact\n");
    printf("  6. Exit\n");
    printf("----------------------------------------------------------------\n\n");
}

void addContact() {
    Contact contact;
    FILE *fp;
    
    clearScreen();
    printf("\n================================================================\n");
    printf("                        ADD NEW CONTACT                         \n");
    printf("================================================================\n\n");
    
    contact.id = getNextId();
    
    printf("Enter Name: ");
    fgets(contact.name, MAX_NAME, stdin);
    contact.name[strcspn(contact.name, "\n")] = 0; 
    
    if(strlen(contact.name) == 0) {
        printf("\n[ERROR] Name cannot be empty!\n");
        pressEnterToContinue();
        return;
    }
    
    printf("Enter Phone Number: ");
    fgets(contact.phone, MAX_PHONE, stdin);
    contact.phone[strcspn(contact.phone, "\n")] = 0;
    
    fp = fopen(FILENAME, "ab");
    if(fp == NULL) {
        printf("\n[ERROR] Unable to open file!\n");
        pressEnterToContinue();
        return;
    }
    
    fwrite(&contact, sizeof(Contact), 1, fp);
    fclose(fp);
    
    printf("\n[SUCCESS] Contact added successfully! (ID: %d)\n", contact.id);
    pressEnterToContinue();
}

void viewAllContacts() {
    Contact contact;
    FILE *fp;
    int count = 0;
    
    clearScreen();
    displayHeader();
    printf("\n                        ALL CONTACTS                            \n");
    printf("----------------------------------------------------------------\n");
    
    fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        printf("No contacts found. Add your first contact!\n");
        pressEnterToContinue();
        return;
    }
    
    printf("%-6s %-30s %-20s\n", "ID", "Name", "Phone Number");
    printf("----------------------------------------------------------------\n");
    
    while(fread(&contact, sizeof(Contact), 1, fp) == 1) {
        count++;
        printf("%-6d %-30s %-20s\n", contact.id, contact.name, contact.phone);
    }
    
    fclose(fp);
    
    if(count == 0) {
        printf("No contacts found.\n");
    } else {
        printf("----------------------------------------------------------------\n");
        printf("Total Contacts: %d\n", count);
    }
    
    pressEnterToContinue();
}

void searchContact() {
    Contact contact;
    FILE *fp;
    char searchTerm[MAX_NAME];
    char lowerName[MAX_NAME], lowerSearch[MAX_NAME];
    int found = 0;
    
    clearScreen();
    printf("\nSEARCH CONTACT\n");
    printf("Enter name or phone to search: ");
    fgets(searchTerm, MAX_NAME, stdin);
    searchTerm[strcspn(searchTerm, "\n")] = 0;
    
    strcpy(lowerSearch, searchTerm);
    toLowerCase(lowerSearch);
    
    fp = fopen(FILENAME, "rb");
    if(fp == NULL) {
        printf("\nNo contacts found.\n");
        pressEnterToContinue();
        return;
    }
    
    printf("\n%-6s %-30s %-20s\n", "ID", "Name", "Phone Number");
    while(fread(&contact, sizeof(Contact), 1, fp) == 1) {
        strcpy(lowerName, contact.name);
        toLowerCase(lowerName);
        if(strstr(lowerName, lowerSearch) != NULL || strstr(contact.phone, searchTerm) != NULL) {
            found = 1;
            printf("%-6d %-30s %-20s\n", contact.id, contact.name, contact.phone);
        }
    }
    fclose(fp);
    if(!found) printf("No matching results.\n");
    pressEnterToContinue();
}

void editContact() {
    Contact contact;
    FILE *fp, *tempFp;
    int id, found = 0;
    
    clearScreen();
    printf("\nEDIT CONTACT\n");
    printf("Enter Contact ID to edit: ");
    if(scanf("%d", &id) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();
    
    fp = fopen(FILENAME, "rb");
    tempFp = fopen("temp.dat", "wb");
    
    if(!fp || !tempFp) { printf("File error.\n"); return; }
    
    while(fread(&contact, sizeof(Contact), 1, fp) == 1) {
        if(contact.id == id) {
            found = 1;
            printf("Enter New Name: ");
            fgets(contact.name, MAX_NAME, stdin);
            contact.name[strcspn(contact.name, "\n")] = 0;
            printf("Enter New Phone: ");
            fgets(contact.phone, MAX_PHONE, stdin);
            contact.phone[strcspn(contact.phone, "\n")] = 0;
        }
        fwrite(&contact, sizeof(Contact), 1, tempFp);
    }
    fclose(fp); fclose(tempFp);
    remove(FILENAME); rename("temp.dat", FILENAME);
    if(found) printf("[SUCCESS] Updated.\n"); else printf("[ERROR] Not found.\n");
    pressEnterToContinue();
}

void deleteContact() {
    Contact contact;
    FILE *fp, *tempFp;
    int id, found = 0;
    char confirm;
    
    clearScreen();
    printf("\nDELETE CONTACT\n");
    printf("Enter Contact ID to delete: ");
    if(scanf("%d", &id) != 1) { clearInputBuffer(); return; }
    clearInputBuffer();
    
    fp = fopen(FILENAME, "rb");
    if(!fp) return;
    
    tempFp = fopen("temp.dat", "wb");
    while(fread(&contact, sizeof(Contact), 1, fp) == 1) {
        if(contact.id == id) {
            found = 1;
            printf("Delete %s? (y/n): ", contact.name);
            scanf(" %c", &confirm);
            if(confirm == 'y' || confirm == 'Y') continue; 
        }
        fwrite(&contact, sizeof(Contact), 1, tempFp);
    }
    fclose(fp); fclose(tempFp);
    remove(FILENAME); rename("temp.dat", FILENAME);
    printf(found ? "[SUCCESS] Deleted.\n" : "[ERROR] Not found.\n");
    pressEnterToContinue();
}



int getNextId() {
    Contact contact;
    FILE *fp = fopen(FILENAME, "rb");
    int maxId = 0;
    if(!fp) return 1;
    while(fread(&contact, sizeof(Contact), 1, fp) == 1) {
        if(contact.id > maxId) maxId = contact.id;
    }
    fclose(fp);
    return maxId + 1;
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void pressEnterToContinue() {
    printf("\nPress Enter to continue...");
    getchar();
}

void toLowerCase(char *str) {
    for(int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}