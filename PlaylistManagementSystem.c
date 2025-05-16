#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SONGS 100
#define MAX_PLAYLISTS 100
#define MAX_NAME_LENGTH 100
#define CLEAR_SCREEN() system("cls")  

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

typedef struct {
    char name[MAX_NAME_LENGTH];
    float rating;
    char songs[MAX_SONGS][MAX_NAME_LENGTH];
    int song_count;
} Playlist;

// Global variables
Playlist playlists[MAX_PLAYLISTS];
int playlist_count = 0;

// Function prototypes
void savePlaylists();
void loadPlaylists();
void addPlaylist();
void removePlaylist();
void viewPlaylists();
void managePlaylist(int index);
void mainMenu();
void printHeader(const char* text);
void printSuccess(const char* message);
void printError(const char* message);
void printWarning(const char* message);
int getValidatedInteger(const char* prompt, int min, int max);
float getValidatedFloat(const char* prompt, float min, float max);
void getValidatedString(const char* prompt, char* dest, int max_length);
void addSongToPlaylist(int index);
void removeSongFromPlaylist(int index);
void changePlaylistRating(int index);

void savePlaylists() {
    FILE *file = fopen("playlists.dat", "wb");
    if (!file) {
        printError("Error saving playlists");
        return;
    }
    
    fwrite(&playlist_count, sizeof(int), 1, file);
    for (int i = 0; i < playlist_count; i++) {
        fwrite(&playlists[i], sizeof(Playlist), 1, file);
    }
    
    fclose(file);
    printSuccess("Playlists saved successfully!");
}

void loadPlaylists() {
    FILE *file = fopen("playlists.dat", "rb");
    if (!file) {
        return;
    }
    
    fread(&playlist_count, sizeof(int), 1, file);
    for (int i = 0; i < playlist_count; i++) {
        fread(&playlists[i], sizeof(Playlist), 1, file);
    }
    
    fclose(file);
}

void addPlaylist() {
    if (playlist_count >= MAX_PLAYLISTS) {
        printError("Maximum playlists reached!");
        return;
    }
    
    Playlist newPlaylist;
    getValidatedString("Enter playlist name (max 99 characters):", newPlaylist.name, MAX_NAME_LENGTH - 1);
    newPlaylist.rating = getValidatedFloat("Enter rating (0-5):", 0, 5);
    newPlaylist.song_count = 0;
    
    playlists[playlist_count++] = newPlaylist;
    savePlaylists();
    printSuccess("Playlist added successfully!");
}

void removePlaylist() {
    if (playlist_count == 0) {
        printError("No playlists available to remove!");
        return;
    }
    
    CLEAR_SCREEN();
    viewPlaylists();
    int plNum = getValidatedInteger("\nEnter playlist number to remove (0 to cancel):", 
                                  0, playlist_count);
    if (plNum == 0) return;
    
    // Confirm deletion
    printf("\nAre you sure you want to delete playlist '%s'? (y/n): ", playlists[plNum-1].name);
    char confirm;
    scanf(" %c", &confirm);
    if (tolower(confirm) != 'y') {
        printWarning("Playlist deletion cancelled.");
        return;
    }
    
    // Shift all playlists after the removed one to fill the gap
    for (int i = plNum-1; i < playlist_count-1; i++) {
        playlists[i] = playlists[i+1];
    }
    
    playlist_count--;
    savePlaylists();
    printSuccess("Playlist removed successfully!");
}

void viewPlaylists() {
    if (playlist_count == 0) {
        printWarning("No playlists available.");
        return;
    }
    
    printHeader("YOUR PLAYLISTS");
    for (int i = 0; i < playlist_count; i++) {
        printf("\n%d. " COLOR_CYAN "%s" COLOR_RESET " (Rating: " COLOR_YELLOW "%.2f" COLOR_RESET ")\n", 
               i+1, playlists[i].name, playlists[i].rating);
        printf("Songs (%d):\n", playlists[i].song_count);
        for (int j = 0; j < playlists[i].song_count; j++) {
            printf("  - " COLOR_GREEN "%s" COLOR_RESET "\n", playlists[i].songs[j]);
        }
    }
}

void addSongToPlaylist(int index) {
    if (playlists[index].song_count >= MAX_SONGS) {
        printError("Playlist is full!");
        return;
    }
    
    getValidatedString("Enter song name (max 99 characters):", 
                      playlists[index].songs[playlists[index].song_count], 
                      MAX_NAME_LENGTH - 1);
    playlists[index].song_count++;
    savePlaylists();
    printSuccess("Song added successfully!");
}

void removeSongFromPlaylist(int index) {
    if (playlists[index].song_count == 0) {
        printError("Playlist is already empty!");
        return;
    }
    
    printf("Current songs in playlist:\n");
    for (int i = 0; i < playlists[index].song_count; i++) {
        printf("%d. %s\n", i+1, playlists[index].songs[i]);
    }
    
    int songNum = getValidatedInteger("Enter song number to remove (0 to cancel):", 
                                    0, playlists[index].song_count);
    if (songNum == 0) return;
    
    for (int i = songNum-1; i < playlists[index].song_count-1; i++) {
        strcpy(playlists[index].songs[i], playlists[index].songs[i+1]);
    }
    
    playlists[index].song_count--;
    savePlaylists();
    printSuccess("Song removed successfully!");
}

void changePlaylistRating(int index) {
    playlists[index].rating = getValidatedFloat("Enter new rating (0-5):", 0, 5);
    savePlaylists();
    printSuccess("Rating updated successfully!");
}

void managePlaylist(int index) {
    char input;
    do {
        CLEAR_SCREEN();
        printf("=== MANAGING PLAYLIST: " COLOR_CYAN "%s" COLOR_RESET " ===\n", playlists[index].name);
        printf("1. Add Song\n");
        printf("2. Remove Song\n");
        printf("3. Change Rating\n");
        printf("4. Back to Main Menu\n");
        printf("Choice: ");
        
        scanf(" %c", &input);
        
        switch(input) {
            case '1':
                CLEAR_SCREEN();
                addSongToPlaylist(index);
                printf("Press any key to continue...");
                getchar(); getchar();
                break;
                
            case '2':
                CLEAR_SCREEN();
                removeSongFromPlaylist(index);
                printf("Press any key to continue...");
                getchar(); getchar();
                break;
                
            case '3':
                CLEAR_SCREEN();
                changePlaylistRating(index);
                printf("Press any key to continue...");
                getchar(); getchar();
                break;
                
            case '4':
                CLEAR_SCREEN();
                return; 
                
            default:
                printError("Invalid choice!");
                printf("Press any key to try again...");
                getchar(); getchar();
        }
    } while (1);
}

void printHeader(const char* text) {
    printf(COLOR_MAGENTA "\n=== %s ===\n" COLOR_RESET, text);
}

void printSuccess(const char* message) {
    printf(COLOR_GREEN "[SUCCESS] %s\n" COLOR_RESET, message);
}

void printError(const char* message) {
    printf(COLOR_RED "[ERROR] %s\n" COLOR_RESET, message);
}

void printWarning(const char* message) {
    printf(COLOR_YELLOW "[WARNING] %s\n" COLOR_RESET, message);
}

int getValidatedInteger(const char* prompt, int min, int max) {
    int value;
    while (1) {
        printf("%s ", prompt);
        if (scanf("%d", &value)) {
            if (value >= min && value <= max) {
                return value;
            }
        }
        printError("Invalid input! Please try again.");
        
        while (getchar() != '\n');
    }
}

float getValidatedFloat(const char* prompt, float min, float max) {
    float value;
    while (1) {
        printf("%s ", prompt);
        if (scanf("%f", &value)) {
            if (value >= min && value <= max) {
                return value;
            }
        }
        printError("Invalid input! Please try again.");
        
        while (getchar() != '\n');
    }
}

void getValidatedString(const char* prompt, char* dest, int max_length) {
    printf("%s ", prompt);
    while (getchar() != '\n'); // Clear input buffer
    
    if (fgets(dest, max_length, stdin)) {
        // Remove trailing newline if present
        size_t len = strlen(dest);
        if (len > 0 && dest[len-1] == '\n') {
            dest[len-1] = '\0';
        }
    }
}

void mainMenu() {
    int choice;
    do {
        CLEAR_SCREEN();
        printHeader("PLAYLIST MANAGER");
        printf("1. Add Playlist\n");
        printf("2. View Playlists\n");
        printf("3. Manage Playlist\n");
        printf("4. Remove Playlist\n");
        printf("5. Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printError("Invalid input!");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        switch(choice) {
            case 1:
                CLEAR_SCREEN();
                addPlaylist();
                printf("Press any key to return to main menu...");
                getchar(); getchar();
                break;
                
            case 2:
                CLEAR_SCREEN();
                viewPlaylists();
                printf("\nPress any key to return to main menu...");
                getchar(); getchar();
                break;
                
            case 3:
                CLEAR_SCREEN();
                if (playlist_count > 0) {
                    viewPlaylists();
                    int plNum = getValidatedInteger("\nEnter playlist number to manage (0 to cancel):", 
                                                   0, playlist_count);
                    if (plNum > 0) {
                        managePlaylist(plNum-1);
                    }
                } else {
                    printWarning("No playlists available to manage!");
                    printf("Press any key to return to main menu...");
                    getchar(); getchar();
                }
                break;
                
            case 4:
                CLEAR_SCREEN();
                removePlaylist();
                printf("Press any key to return to main menu...");
                getchar(); getchar();
                break;
                
            case 5:
                printf("Goodbye!\n");
                exit(0);
                
            default:
                printError("Invalid choice!");
                printf("Press any key to try again...");
                getchar(); getchar();
        }
    } while (1);
}

int main() {
    loadPlaylists();
    mainMenu();
    return 0;
}