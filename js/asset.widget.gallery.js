(function ($, Drupal) {

Drupal.behaviors.asset_widget_gallery = {
  attach: function(context, settings) {
    $('.asset-gallery', context).once(function(){
      var $this = $(this);
      var $imageFull = $this.find('.asset-gallery-full img:first');
      var $children = $this.find('.asset-gallery-thumbs a');
      $children.first().addClass('active');
      $children.on('click', function(e){
        e.preventDefault();
        var $image = $(this).find('img');
        $children.removeClass('active');
        $(this).addClass('active');
        $imageFull.attr('src', $(this).attr('href'));
      });
    });
  }
};

})(jQuery, Drupal);
