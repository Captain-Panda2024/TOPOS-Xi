import * as vscode from 'vscode';

export function activate(context: vscode.ExtensionContext) {
    console.log('TOPOS-Ξ extension is now active!');

    let disposable = vscode.commands.registerCommand('topos-xi.helloWorld', () => {
        vscode.window.showInformationMessage('Hello from TOPOS-Ξ!');
    });

    context.subscriptions.push(disposable);
}

export function deactivate() {}
