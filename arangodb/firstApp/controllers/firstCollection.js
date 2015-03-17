(function () {
  'use strict';
  var Foxx = require('org/arangodb/foxx'),
    ArangoError = require('org/arangodb').ArangoError,
    firstCollection_repo = require('repositories/firstCollection').Repository,
    FirstCollection = require('models/FirstCollection').Model,
    joi = require('joi'),
    _ = require('underscore'),
    controller,
    FirstCollectionDescription = {
      type: joi.string().required().description(
        'The id of the FirstCollection-Item'
      ),
      allowMultiple: false
    },
    FirstCollection_repo;

  controller = new Foxx.Controller(applicationContext);

  FirstCollection_repo = new firstCollection_repo(applicationContext.collection('firstCollection'), {
    model: FirstCollection
  });

  /** Lists of all FirstCollection
   *
   * This function simply returns the list of all FirstCollection.
   */
  controller.get('/firstCollection', function (req, res) {
    res.json(_.map(FirstCollection_repo.all(), function (model) {
      return model.forClient();
    }));
  });

  /** Creates a new FirstCollection
   *
   * Creates a new FirstCollection-Item. The information has to be in the
   * requestBody.
   */
  controller.post('/firstCollection', function (req, res) {
    var firstCollection = req.params('firstCollection');
    res.json(FirstCollection_repo.save(firstCollection).forClient());
  }).bodyParam('firstCollection', {
    description: 'The FirstCollection you want to create',
    type: FirstCollection
  });

  /** Reads a FirstCollection
   *
   * Reads a FirstCollection-Item.
   */
  controller.get('/firstCollection/:id', function (req, res) {
    var id = req.params('id');
    res.json(FirstCollection_repo.byId(id).forClient());
  }).pathParam('id', FirstCollectionDescription);

  /** Updates a FirstCollection
   *
   * Changes a FirstCollection-Item. The information has to be in the
   * requestBody.
   */
  controller.put('/firstCollection/:id', function (req, res) {
    var id = req.params('id'),
    firstCollection = req.params('firstCollection');
    res.json(FirstCollection_repo.replaceById(id, firstCollection));
  }).pathParam('id', FirstCollectionDescription
  ).bodyParam('firstCollection', 'The FirstCollection you want your old one to be replaced with', FirstCollection);

  /** Removes a FirstCollection
   *
   * Removes a FirstCollection-Item.
   */
  controller.del('/firstCollection/:id', function (req, res) {
    var id = req.params('id');
    FirstCollection_repo.removeById(id);
    res.json({ success: true });
  }).pathParam('id', FirstCollectionDescription
  ).errorResponse(ArangoError, 404, 'The document could not be found');
}());

