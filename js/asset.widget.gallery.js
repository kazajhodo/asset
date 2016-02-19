(function ($, Drupal) {

Drupal.behaviors.asset_widget_gallery = {
  attach: function(context, settings) {
    $('.asset-gallery', context).once(function(){
      var $this = $(this);
      var galleryId = $this.data('asset-gallery');
      if(settings.asset && settings.asset.gallery && settings.asset.gallery[galleryId]){
        var galleryMarkup = settings.asset.gallery[galleryId];
        var $imageFull = $this.find('.asset-gallery-full');
        var $children = $this.find('.asset-gallery-thumbs a');
        var timeout;
        $children.first().addClass('active');
        $children.on('click', function(e){
          var $self = $(this);
          var itemId = $self.data('asset-gallery-id');
          e.preventDefault();
          if(galleryMarkup[itemId]){
            clearTimeout(timeout);
            $imageFull.addClass('animate');
            $children.removeClass('active');
            $self.addClass('active');
            var $markup = $(galleryMarkup[itemId]);
            // Load included images first before switching content.
            $markup.find('img').on('load', function(){
              timeout = setTimeout(function(){
                $imageFull.html($markup);
                Drupal.attachBehaviors($imageFull);
                timeout = setTimeout(function(){
                  $imageFull.removeClass('animate');
                },10);
              }, 300);
            });
          }
        });
      }
    });
  }
};

})(jQuery, Drupal);
