from django.db import models

# Create your models here.
class Test(models.Model):
    name = models.CharField(max_length=20)

class Contact(models.Model):
    name   = models.CharField(max_length=200)
    age    = models.IntegerField(default=0)
    email  = models.EmailField()
    def __unicode__(self):
        return self.name
 
class Tag(models.Model):
    contact = models.ForeignKey(Contact)
    name    = models.CharField(max_length=50)
    def __unicode__(self):
        return self.name