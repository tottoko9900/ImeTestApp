# ImeTestApp
IMEのAPIを使ってみるテストアプリ

## EventHooker
C++で作成されているDll

ウィンドウメッセージをグローバルフックし、IMEのウィンドウメッセージをハンドルしている

C#で使用することを想定

C++20を使用している


## ImeTestApp
EventHookerを使用するアプリ

WPFで作成

EventHookerから送られてくる文字列を表示している
