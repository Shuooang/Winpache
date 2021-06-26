# `Winpache` project
* HTTP Project VC++.
* HTTPS SSL capable.
* DLL project for site API.

# Download
[Download ReleaseWinpache.zip](https://www.dropbox.com/sh/ch86hzxfhbgl7fs/AABdYPPYMteR5SpTsgMEbUeJa?dl=1)

# Start for simple a `Web Server`(WS)
1. Run `WinpacheSetup.msi`
2. Run the "Start Winpache Project" app that is `StartWinpache.exe`
3. Click the `Start Winpache Server` button, then `Winpache` app will run.
4. In `Winpache`, set the web document location and etc configuration.
5. Start the server, then a simple web server will run.

# To setup the `Main DB` for `Winpache`
1. In the database menu group, go through the numbered menus one by one in order.
2. If you stop and start the configured server, the main DB is applied and basic logs are managed.
3. This basic `Main DB` is separate from the `Site DB`.

# Start for a `Web Application Server`(WAS)
1. In `Winpache`, set the web document location and etc configuration.
2. In the `database` menu group, run the `HeidiSQL` and create database and tables.
3. Then, if you click the `Connect to site DB` menu next to the `Restart` menu, the web server is connected to the site server in the site DB.
4. Next, go to the `Project` menu group, go through the numbered menus one by one in order and test the samples and add new `API` functions.
5. Stop and start the server, then a `Web Application Server` will run.

# `Winpache` source
* Only those who study the inside of 'Winpache' need the above source, and the developer who will build the server only needs to download the following Winpache setup files. 
* It also contains the template source to add the API to, and there is a tutorial in the menu.

