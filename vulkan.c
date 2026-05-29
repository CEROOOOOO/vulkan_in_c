#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define max(a, b) ((a) > (b) ? (a) : (b));    // max of two numdbers
int characterReplacement(char *s, int k) {
        // Array to store frequency count of each uppercase letter (A-Z)
        int charFrequency[26] = {0};
      
        // Left pointer of sliding window
        int left = 0;
      
        // Maximum frequency of any single character in current window
        int maxFrequency = 0;
      
        // Length of the string
        int stringLength = strlen(s);
      
        // Iterate through string with right pointer
        for (int right = 0; right < stringLength; ++right) {
            // Increment frequency of current character and update max frequency
            charFrequency[s[right] - 'A']++;
            maxFrequency = max(maxFrequency, charFrequency[s[right] - 'A']);//get the max frequency of the current window and store it in max frequency liek if we have AABABBA and we are at the end of the string
          
            // Current window size: right - left + 1
            // Characters to replace: window size - max frequency
            // If replacements needed > k, shrink window from left
            if (right - left + 1 - maxFrequency > k) {
                // Decrement frequency of leftmost character and move left pointer
                charFrequency[s[left] - 'A']--;
                left++;
            }
        }
      
        // Final window size is the maximum valid substring length
        return stringLength - left;
    }