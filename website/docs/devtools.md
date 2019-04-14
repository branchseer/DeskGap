# Web Developer Tools on Windows

## WinRT WebView

Use [Microsoft Edge DevTools](https://www.microsoft.com/en-us/p/microsoft-edge-devtools-preview/9mzbfrmz0mnj).

## Trident WebView

1. Install a recent version of [Visual Studio](https://visualstudio.microsoft.com/downloads/) (The free “Community” Edition is fine. The following steps are based on VS 2017). Make sure the “ASP.NET and web development” workload is selected in the installer;

2. Open **Debug** -> **Attach to Process...**;

3. Click **Select...** and choose **Debug there code types:** -> **Script**

4. Find and select the process of your DeskGap application in **Available Pocesses**;

5. Click **Attach**. The **DOM Explorer** and **JavaSript Console** should now show up. If not, open them in **Debug** -> **Windows**.
