const greetModule = require ('./build/Release/greet.node')

console.log('exports : ', greetModule)
console.log()

greetModule.playChess()
console.log()