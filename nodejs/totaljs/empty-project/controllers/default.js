exports.install = function() {
    // define routes with actions
    F.route('/', view_index);
    F.route('/services/{name}/', view_services);
    F.route('/contact/', view_contact);
    F.route('/contact/', json_contact, ['post']);
}

// The action
function view_index() {
    var self = this;
    // The "index" view is routed into the views/index.html
    // ---> Send the response
    self.view('index');
}

function view_services(name) {
    var self = this;
    // The "services" view is routed into the views/services.html
    // A second argument is the model
    // ---> Send the response
    self.view('services', { category: name });
}

function view_contact() {
    var self = this;
    // "contact" view is routed to views/contact.html
    // ---> Send the response
    self.view('contact');
}

function json_contact() {
    var self = this;

    // Get the data from the request body.
    // The data are parsed into the object automatically.
    var model = self.body;

    // e.g.
    // model.email
    // model.name

    // Send the mail to our company
    var message = self.mail('info@company.com', 'Contact form', 'mail-template', model);
    message.reply(model.email);

    // ---> and send the response in JSON format
    self.json({ success: true });
}
