# Hacking the authentication logic in Cheat Engine Patreon

**Disclaimer: This program is made only for study purpose, do not use it for illegal activities!**

# How does it work?

Cheat Engine Patreon's installer will check your patreon account for membership before it let you proceed with installation. 

For people who own Cheat Engine Patreon, you might notice that Cheat Engine stops asking you for authentication in the next 30 days, allowing you to do infinite reinstalls without authentication within this time period. 

This tool abuses this machenics by encoding the current system date in the expected format and write it into the registry. 
This will make Cheat Engine Patreon installer think that you have already authenticated Cheat Engine Patreon in the last 30 days and allows you to proceed with the installation. 

