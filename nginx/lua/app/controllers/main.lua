local main = {}

function main.index(page)
    page:render('index')
end

function main.hello(page)
    page:write( "Hello world!")
end

return main
